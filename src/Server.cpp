#include "Map.h"
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
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>
#include<sstream>
#include "RDB Reader/RDBParser.hpp"

std::mutex mutex_guard;
std ::string to_lower(std ::string s) {
  std ::string ans;
  for (auto x : s) {

    ans += (char)tolower(x);
  }
  return ans;
}

long get_current_time_ms() {
  auto time_now = std::chrono::system_clock::now();
  auto now_in_ms =
      std::chrono::time_point_cast<std::chrono::milliseconds>(time_now);
  auto value = now_in_ms.time_since_epoch();
  long current_time_in_ms = value.count();
  return current_time_in_ms;
}
void handle_connect(
    int client_fd, int argc, char **argv,
    std::vector<std::vector<std::string> > additional_pair) {
  std ::unique_ptr<In_Memory_Storage> key_value_storage{
      std::make_unique<In_Memory_Storage>()};

  long current_time = get_current_time_ms();
  for (auto it : additional_pair) {
        if(it[2] == "-1"){
          key_value_storage->set(it[0], it[1], current_time + 999999999999);
        }
        else {
            // int64_t unixTime;
            // std::stringstream ss1(it[2]);
            // ss1 >> unixTime;
            //std::cout << current_time <<" " << stol(it[2]) << std::endl;
            key_value_storage->set(it[0], it[1],  stol(it[2]));
        }
      }
  //std ::cout << additional_pair.size() << std::endl;
  for (int i = 1; i < argc; i += 2) {
    if (i + 1 < argc) {
      std::string key = argv[i];
      std::string value = argv[i + 1];
      long current_time_in_ms = get_current_time_ms();
      key_value_storage->set(key.substr(2), value,
                             current_time_in_ms + 999999999999);
    }
  }
  while (true) {
    char msg[1024] = {};
    int rc = recv(client_fd, &msg, sizeof(msg), 0);

    if (rc <= 0) {
      close(client_fd);
      break;
    }
    // std::lock_guard<std::mutex> guard(mutex_guard);
    std ::string header = msg;

    std ::string response = "";

    std ::unique_ptr<Redis> parser{std::make_unique<Redis>(header)};

    std ::vector<std ::string> parser_list = parser->get_command(header);

    if (parser_list[0] == "PING") {
      response = "+PONG\r\n";
    } else if (parser_list[0] == "SET") {
      long current_time_in_ms = get_current_time_ms();
      // std :: cout <<  "Time is " << current_time_in_ms << std :: endl;
      // std :: cout <<  "Time is " << current_time_in_ms +
      // stoi(parser_list.back()) << std :: endl;
      response = "+OK\r\n";
      if (parser_list.size() > 3)
        key_value_storage->set(parser_list[1], parser_list[2],
                               current_time_in_ms + stoi(parser_list.back()));
      else
        key_value_storage->set(parser_list[1], parser_list[2],
                               current_time_in_ms + 999999999999);

      // std :: cout << "size is " << parser->getSize() << std :: endl;
    } else if (parser_list[0] == "GET") {
      long current_time_in_ms = get_current_time_ms();
      // std :: cout <<  "Time is " << current_time_in_ms << std :: endl;
      response = key_value_storage->get(parser_list[1], current_time_in_ms);
      //std ::cout << response << std ::endl;
      if (response == "") {
        response = "$-1\r\n";
      } else {
        response =
            "$" + std::to_string(response.size()) + "\r\n" + response + "\r\n";
      }
    } else if (parser_list[0] == "CONFIG") {
      if (parser_list[1] == "GET") {
        std ::cout << "Key is " + parser_list[2] << std ::endl;
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

    } else {
      for (int i = 1; i < parser_list.size(); i++) {
        response += '$';
        response += std::to_string(parser_list[i].size());
        response += "\r\n";
        response += parser_list[i] + "\r\n";
      }
    }
    std ::cout << "Response is " << response << std ::endl;

    // std :: cout <<

    send(client_fd, response.c_str(), response.length(), 0);

    // std :: cout << ans.size() << std :: endl;
    // std :: cout << ans[0]  << std :: endl ;

    // guard.unlock();
    //
  }
  close(client_fd);
}

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // for (int i = 0; i < argc; i++) {
  //   std ::cout << argv[i] << std ::endl;
  // }

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

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }

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
    RDBParser* rdbParser = new RDBParser();
    v = rdbParser->read_path(endpoint);
  }

  while (true) {

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                           (socklen_t *)&client_addr_len);

    std::cout << "Client connected\n";

    std::thread th(handle_connect, client_fd, argc, argv, v);

    th.detach();
  }
  close(server_fd);
  return 0;
}