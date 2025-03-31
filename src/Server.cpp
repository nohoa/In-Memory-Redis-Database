#include "Map.h"
#include "RDB Reader/RDBParser.hpp"
#include "Redis.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>

extern int send_request1 (std:: string& port,std:: string&replica_no,struct sockaddr_in& server_addr , int server_fd, int argc, char**argv);
std::mutex mutex_guard;
std::mutex mtx;
 std ::string to_lower(std ::string s) {
  std ::string ans;
  for (auto x : s) {

    ans += (char)tolower(x);
  }
  return ans;
}
std ::unique_ptr<In_Memory_Storage> key_value_storage{
  std::make_unique<In_Memory_Storage>()};

 long get_current_time_ms() {
  auto time_now = std::chrono::system_clock::now();
  auto now_in_ms =
      std::chrono::time_point_cast<std::chrono::milliseconds>(time_now);
  auto value = now_in_ms.time_since_epoch();
  long current_time_in_ms = value.count();
  return current_time_in_ms;
}
 std::vector<std::string>  set_client1 ;
 std :: vector<int> replica_id1 ;
 int replica_count = 0 ;

 void handle_connect(int client_fd, int argc, char **argv,
                    std::vector<std::vector<std::string> > additional_pair) {
                     // std :: cout << "here" << std::endl;
        //std :: cout << mutex_guard.try_lock() << std::endl;
      //while(mutex_guard.try_lock() == false ) ;
      //std :: cout << "mutex finished" << std :: endl;
  long current_time = get_current_time_ms();
  std :: string master_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
  for (auto it : additional_pair) {
    if (it[2] == "-1") {
      key_value_storage->set(it[0], it[1], current_time + 999999999999);
    } else {

      key_value_storage->set(it[0], it[1], stol(it[2]));
    }
  }
  for (int i = 1; i < argc; i += 2) {
    if (i + 1 < argc) {
      std::string key = argv[i];
      std::string value = argv[i + 1];
      long current_time_in_ms = get_current_time_ms();
      key_value_storage->set(key.substr(2), value,
                             current_time_in_ms + 999999999999);
    }
  }
  int sz = 0 ;
  while (true) {
   // std :: cout << "new command" << std :: endl;
    char msg[1024] = {};
    int rc = recv(client_fd, &msg, sizeof(msg), 0);

    //std::cout << rc << std:: endl;
    //std :: cout << rc << std:: endl;
    //std :: cout << std::endl;

    if (rc <= 0) {
      //std :: cout << "new cmd" << std :: endl;
      //std :: cout << set_client.size() << std::endl;
      std :: string res = "$1\r\na";
      send(client_fd, res.c_str(), res.length(), 0);
      for(auto it : set_client1){
        //std :: cout << it << std :: endl;
        send(client_fd, it.c_str(), it.length(), 0);
      }
      //close(client_fd);
      break;
    }
    // std::lock_guard<std::mutex> guard(mutex_guard);
    std ::string header = msg;


    std ::string response = "";

    std ::unique_ptr<Redis> parser{std::make_unique<Redis>(header)};

    std ::vector<std ::string> parser_list = parser->get_command(header);

    std :: vector<std::string > all_cmd  = parser->get_client_command(header);

    std :: cout << std::endl;

    if (parser_list[0] == "PING") {
      response = "+PONG\r\n";
      //std :: cout << "go here " << std::endl;
    } else if (parser_list[0] == "SET") {

     // std :: cout << "here ? " << std :: endl;
     mutex_guard.lock();
      long current_time_in_ms = get_current_time_ms();

      response = "+OK\r\n";
      //std :: cout << parser_list[1] <<" " << parser_list[2] << std :: endl;
      if (parser_list.size() > 3)
        key_value_storage->set(parser_list[1], parser_list[2],
                               current_time_in_ms + stoi(parser_list.back()));
      else
        key_value_storage->set(parser_list[1], parser_list[2],
                               current_time_in_ms + 999999999999);
        
      //int status = send(client_fd, response.c_str(), response.length(), 0);
      //response = "$";
      std :: string set_response ="*" ;
      set_response += std::to_string(all_cmd.size());
      set_response += "\r\n";
      for(auto it : all_cmd) {
        set_response += "$";
        set_response += std::to_string(it.size());
        set_response += "\r\n";
        set_response += it;
        set_response += "\r\n";
      }         
      for(int i = 0 ;i < replica_id1.size() ;i ++){
       // std :: cout << replica_id[i] << std :: endl;
        write(replica_id1[i],set_response.c_str(),set_response.size());
      }
      mutex_guard.unlock();
      //write(client_fd,set_response.c_str(),set_response.size());
      //std :: cout << "here ? " << std :: endl;
      // response += std::to_string(all_response.length());
      // response += "\r\n" ;
      // response += all_response;
      // response += "\r\n";
    } else if (parser_list[0] == "GET") {
      //std :: cout << "??" << std :: endl;
      
      mutex_guard.lock();
      long current_time_in_ms = get_current_time_ms();

      response = key_value_storage->get(parser_list[1], current_time_in_ms);
     // std :: cout << "lol" << std :: endl;
      //std :: cout << "response is " << response << std :: endl;
      mutex_guard.unlock();

      if (response == "") {
        response = "$-1\r\n";
      } else {
        response =
            "$" + std::to_string(response.size()) + "\r\n" + response + "\r\n";
      }
    } else if (parser_list[0] == "CONFIG") {
      if (parser_list[1] == "GET") {
       // std :: cout << replica_id[0] << std :: endl;
        response = key_value_storage->get(parser_list[2], 0);
        if (response == "") {
          response = "$-1\r\n";
        } else {
          response = "*2\r\n$" + std::to_string(parser_list[2].length()) +
                     "\r\n" + parser_list[2] + "\r\n$" +
                     std::to_string(response.size()) + "\r\n" + response +
                     "\r\n";
        }
      }
    } else if (parser_list[0] == "KEYS") {

      std::vector<std::string> keys = key_value_storage->getAllKey();
      int sz = keys.size();
      response.clear();
      response = '*' + (std::to_string(sz)) + "\r\n";
      for (auto it : keys) {
        response += '$';
        response += (std::to_string(it.size())) + "\r\n";
        response += it + "\r\n";
      }

    } else if (parser_list[0] == "INFO") {

      response = "$11\r\nrole:master\r\n";
      // bool is_replication = false;
      if (argc >= 5) {
        std ::string replica = argv[3];
        if (replica.compare("--replicaof") == 0) {
          // is_replication = true ;

          response = "$10\r\nrole:slave\r\n";
        }
      }
      //std ::cout << "response here" << std::endl;
      std ::string replied_id =
          "master_replid:8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
      std ::string master_repl_offset = "master_repl_offset:0";
      std::vector<std::string> replications;
      replications.push_back(response);
      replications.push_back(replied_id);
      replications.push_back(master_repl_offset);
      int sz = 0;
      for(auto it : replications){
        sz += it.size();
      }
      sz += 2*(replications.size()-1);
      std :: string current = response ;
      response.clear();
      response += '$';
      response += std::to_string(sz);
      response +="\r\n";
      //std :: cout << response << std::endl;
      for (auto it : replications) {
        //std :: cout << it << std::endl;
        //response += std::to_string(it.length());
        //response += "\r\n";
        response += it;
        response += "\r\n";
      }
    } 
    else if(parser_list[0] == "REPLCONF"){
      std :: cout << "refffff" << std::endl;
      for(auto it : parser_list){
        std :: cout << it <<" ";
      }
      std :: cout << std::endl;
        if(parser_list[1] != "ACK")  response = "$2\r\nOK\r\n";
        else {
          
          replica_count ++;
          response = "*3\r\n$8\r\nREPLCONF\r\n$3\r\nACK\r\n$1\r\n0\r\n";
        }
        std :: cout << replica_count << std::endl;
    }
    else if(parser_list[0] == "PSYNC"){
      //std :: cout << "synccc" << std :: endl;
      replica_id1.push_back(client_fd);
      std ::string response_str = "+FULLRESYNC 8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb 0\r\n ";
      response = "$" + std::to_string(response_str.size()) +"\r\n"+response_str + "\r\n";
      send(client_fd, response.c_str(), response.length(), 0);

      std:: string empty_RDB_file =  "\x52\x45\x44\x49\x53\x30\x30\x31\x31\xfa\x09\x72\x65\x64\x69\x73\x2d\x76\x65\x72\x05\x37\x2e\x32\x2e\x30\xfa\x0a\x72\x65\x64\x69\x73\x2d\x62\x69\x74\x73\xc0\x40\xfa\x05\x63\x74\x69\x6d\x65\xc2\x6d\x08\xbc\x65\xfa\x08\x75\x73\x65\x64\x2d\x6d\x65\x6d\xc2\xb0\xc4\x10\x00\xfa\x08\x61\x6f\x66\x2d\x62\x61\x73\x65\xc0\x00\xff\xf0\x6e\x3b\xfe\xc0\xff\x5a\xa2";

        response  =  "$"+ std::to_string(empty_RDB_file.size()) +"\r\n"+empty_RDB_file ;

    }
    else if(parser_list[0] == "WAIT"){
      //std :: cout << "wait ?" << std::endl;
       response = ":"+std::to_string(replica_count) + "\r\n";
       std :: string ack = "*3\r\n$8\r\nREPLCONF\r\n$6\r\nGETACK\r\n$1\r\n*\r\n";
       for(auto it : replica_id1) {
         send(it,ack.c_str(),ack.length(),0);

       }


     // send(client_fd, response.c_str(), response.length(), 0);
      //response = "*3\r\n$8\r\nREPLCONF\r\n$3\r\nACK\r\n$1\r\n0\r\n.";
    }
    else {
      for (int i = 1; i < parser_list.size(); i++) {
        response += '$';
        response += std::to_string(parser_list[i].size());
        response += "\r\n";
        response += parser_list[i] + "\r\n";
      }

    }
   if(response.size() > 0) send(client_fd, response.c_str(), response.length(), 0);
    //std :: cout << "finish" << std :: endl;
  }
  //std :: cout << "ed" << std::endl;

  // sz = 0;
  // //std :: cout << "end" << std::endl;
  
  // std:: string empty_RDB_file = "UkVESVMwMDEx+glyZWRpcy12ZXIFNy4yLjD6CnJlZGlzLWJpdHPAQPoFY3RpbWXCbQi8ZfoIdXNlZC1tZW3CsMQQAPoIYW9mLWJhc2XAAP/wbjv+wP9aog==";

  // std :: string file_response  =  "$";

  //send(client_fd, file_response.c_str(), file_response.length(), 0);

  //std :: cout << file_response.size() << std :: endl ;
  
 // while((sz = send(client_fd, file_response.c_str(), file_response.length(), 0))  == 0 );


  close(client_fd);
}

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // for (int i = 0; i < argc; i++) {
  //   std ::cout << argv[i] << std ::endl;
  // }
  std::vector<std::string> argument ;
  for(int i = 0 ;i  < argc ;i ++){
    argument.push_back(argv[i]);
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);

  struct sockaddr_in replica_server_addr;
  if (argc == 3) {
    std ::string port_exist = argv[1];
    if (port_exist.compare("--port") == 0) {
      int port_no = (std::stoi)(argv[2]);
      // std :: cout << port_no << std :: endl;
      server_addr.sin_port = htons(port_no);
    }
  }
  else if(argc >= 5 && argument[3].compare("--replicaof") == 0){

    std ::string bin_key;
  std ::string bin_value;
  std::vector<std::vector<std::string> > v;

    std::string endpoint = "";
    endpoint += argv[2];
    endpoint += '/';
    endpoint += argv[4];
    RDBParser *rdbParser = new RDBParser();
    v = rdbParser->read_path(endpoint);
    std::string port = argv[4];
    std :: string replica_no = argv[2];

    //std ::unique_ptr<Client_Request> client{std::make_unique<Client_Request>()};
    std :: cout << "thread launched" << std::endl;

    std::thread th1(send_request1,std::ref(port),std::ref(replica_no),std::ref(server_addr), server_fd,argc,argv);
    th1.detach();
   //int status = client->send_request(port,replica_no,std::ref(server_addr), server_fd,argc,argv);
   //std :: cout << status << std::endl;
   //std :: cout << server_addr.sin_port << std ::endl;
    //if(status == -1) return -1;
    int port_no = (std::stoi)(argv[2]);
      // std :: cout << port_no << std :: endl;
      server_addr.sin_port = htons(port_no);
    
  }
//  std :: cout << "out here" << std :: endl;

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }

  // else {
  //   server_addr.sin_port =  htons(6380);
  //   if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))
  //   !=
  //     0) {
  //   std::cerr << "Failed to bind to port 6380\n";
  //   return 1;
  //     }
  // }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }

  


  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";
  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!\n";
  // Uncomment this block to pass the first stage
  //
  std ::string bin_key;
  std ::string bin_value;
  std::vector<std::vector<std::string> > v;

  if (argc >= 5) {
    std::string endpoint = "";
    endpoint += argv[2];
    endpoint += '/';
    endpoint += argv[4];
    RDBParser *rdbParser = new RDBParser();
    v = rdbParser->read_path(endpoint);
  }

  //std :: cout << "here ?" << std :: endl;

  while (true) {

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                           (socklen_t *)&client_addr_len);

    std::cout << "Client connected\n";

    std::thread th(handle_connect, client_fd, argc, argv, v);

    th.detach();
  }
  //std :: cout << "here" << std::endl;
  close(server_fd);
  return 0;
}