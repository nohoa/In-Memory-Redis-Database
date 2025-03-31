#include "../Client/Client.hpp"
#include "../Map.h"
#include "../RDB Reader/RDBParser.hpp"
#include "../Redis.h"
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

extern std::mutex mutex_guard;
extern int replica_count ;
extern std ::unique_ptr<In_Memory_Storage> key_value_storage;
   extern long get_current_time_ms() ;
int send_request1(std::string &port, std::string &replica_no,
                  struct sockaddr_in &server_addr, int server_fd, int argc,
                  char **argv) {

  std ::string port_no = "";
  int id = port.size() - 1;
  while (port[id] >= '0' && port[id] <= '9') {
    port_no += port[id];
    id--;
  }
  std::reverse(port_no.begin(), port_no.end());
  // std :: cout << port_no << std::endl ;
  // std :: cout << port << std::endl;
  std :: cout << "here" << std::endl;
  struct sockaddr_in back_up_addr;
  back_up_addr.sin_family = AF_INET;
  back_up_addr.sin_addr.s_addr = INADDR_ANY;
  back_up_addr.sin_port = htons(std::stoi(port_no));
  int backup_fd;
  if ((backup_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std ::cout << "Error  socket creation" << std ::endl;
    return -1;
  }
  std ::cout << "Connecting to master " << std::endl;
  int status;
  if ((status = connect(backup_fd, (struct sockaddr *)&back_up_addr,
                        sizeof(back_up_addr))) < 0) {
    printf("\nConnection Failed \n");
  } else {
    replica_count ++;
    std ::string SEND_PING = "*1\r\n$4\r\nPING\r\n";
    send(backup_fd, SEND_PING.c_str(), SEND_PING.length(), 0);
    char msg[1024] = {0};
    while (recv(backup_fd, &msg, sizeof(msg), 0) <= 0)
      ;
    std ::string SEND_REPLCONF =
        "*3\r\n$8\r\nREPLCONF\r\n$14\r\nlistening-port\r\n$4\r\n6380\r\n";
    send(backup_fd, SEND_REPLCONF.c_str(), SEND_REPLCONF.length(), 0);
    while (recv(backup_fd, &msg, sizeof(msg), 0) <= 0)
      ;
    SEND_REPLCONF = "*3\r\n$8\r\nREPLCONF\r\n$4\r\ncapa\r\n$6\r\npsync2\r\n";
    send(backup_fd, SEND_REPLCONF.c_str(), SEND_REPLCONF.length(), 0);
    while (recv(backup_fd, &msg, sizeof(msg), 0) <= 0)
      ;
    std ::string synch = "*3\r\n$5\r\nPSYNC\r\n$1\r\n?\r\n$2\r\n-1\r\n";
    send(backup_fd, synch.c_str(), synch.length(), 0);
    while (recv(backup_fd, &msg, sizeof(msg), 0) <= 0)
      ;

    std :: string ack = "*3\r\n$8\r\nREPLCONF\r\n$3\r\nACK\r\n$1\r\n0\r\n";
    send(backup_fd,ack.c_str(),ack.length(),0);


    // for(int i  = 0 ;i < sz ;i ++){}
    std :: cout << "slave ready " << std::endl;
    int total_size =  0 ;
    bool ping_exist = false;
    while (true) {
      std ::string all;
      memset(msg, 0, sizeof(msg));
      int sz = recv(backup_fd, &msg, sizeof(msg), 0);
      if (sz <= 0) {
        // close(backup_fd);
        break;
      }
      bool exist  = false;
      for(int i = 0 ; i < sz-2 ; i ++){
        if( msg[i]== 'A' && msg[i+1] == 'C' && msg[i+2] =='K'){
          exist = true;
          break;
        }
      }
      for(int i = 0 ; i < sz-2 ; i ++){
        if( msg[i]== 'P' && msg[i+1] == 'I' && msg[i+2] =='N'){
          ping_exist = true;
          break;
        }
      }
      if(ping_exist) total_size += sz;
      if(exist == true){
        if(ping_exist){
          std :: string curr_size = std::to_string(std::to_string(total_size).length());
          std :: string ack = "*3\r\n$8\r\nREPLCONF\r\n$3\r\nACK\r\n$" + curr_size + "\r\n" + std::to_string(total_size) +"\r\n";
          send(backup_fd,ack.c_str(),ack.length(),0);
        }
      }
      std :: cout << sz << std::endl;
         int start_id = 0;
         while(start_id < sz && msg[start_id] != '*') start_id ++;
      for (int i = start_id; i < sz; i++) {
        //std::cout << msg[i] ;
        if (msg[i] == '*' || msg[i] == '$')
          all += msg[i];
        else if (msg[i] >= 'A' && msg[i] <= 'Z')
          all += msg[i];
        else if (msg[i] >= '0' && msg[i] <= '9')
          all += msg[i];
        else if (msg[i] >= 'a' && msg[i] <= 'z')
          all += msg[i];
      }
      //std :: cout << std::endl;
      int id = 0;
      while(id < all.size()){
          std::string key ;
          std ::string value ;
          while(id < all.size() &&all[id] != 'T'){
             id ++;
          }
          id ++;
          id ++;
          id ++;
          while(id < all.size() && all[id] != '$') {
             key += all[id];
             id ++;
          }
          //id ++;
          id ++;
          id ++;
          while(id < all.size() && all[id] != '*') {
             value += all[id];
             id ++;
          }
         // std::vector<std::string > vv;
          //vv.push_back(key);
          //vv.push_back(value);
          //vv.push_back("-1");
          //std :: cout << key << " " << value << std::endl;
          mutex_guard.lock();
          long currtime = get_current_time_ms();
          key_value_storage->set(key, value, currtime +999999999999);
          mutex_guard.unlock();

         // v.push_back(vv);
          //id ++;
      }
      // std :: cout << msg[0] << std::endl;
      // std :: cout << total_size << std :: endl;
    }
    std :: cout << "finished" << std::endl;
    //std ::cout << all << std::endl;
    // std::cout << sz << std::endl;
    // std  :: cout << "go here" << std :: endl;
    //   while(true){
    //     int
    //   }
    //   while(true){
    //     memset(msg,0,sizeof(msg));
    //   }
    // sz = recv(backup_fd,&msg,sizeof(msg),0);
    //   std :: cout << std::endl;
    // std :: cout << "real ? " << std::endl;
    // std :: cout << "out here" << std :: endl;

    // close(backup_fd);
  }
  return 0;
}