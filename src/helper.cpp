#include <arpa/inet.h>
#include <chrono>
#include <condition_variable>
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
#include <queue>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <utility>
#include <vector>

std::condition_variable wait_cond;
std::mutex wait_mutex;
int m_wait_count  = 0;
std ::vector<int> replica_id;

long get_current_time_ms() {
    auto time_now = std::chrono::system_clock::now();
    auto now_in_ms =
        std::chrono::time_point_cast<std::chrono::milliseconds>(time_now);
    auto value = now_in_ms.time_since_epoch();
    long current_time_in_ms = value.count();
    return current_time_in_ms;
  }

  void send_ack(int client_fd, std::string count, std::string wait_time) {

    std::unique_lock<std::mutex> lock(wait_mutex);
    m_wait_count = std::stoi(count);
    std ::string ack = "*3\r\n$8\r\nREPLCONF\r\n$6\r\nGETACK\r\n$1\r\n*\r\n";
    for (auto it : replica_id) {
      send(it, ack.c_str(), ack.length(), 0);
    }
    auto curr_time = std::chrono::system_clock::now() +
                     std::chrono::milliseconds(std::stoi(wait_time));
  
    wait_cond.wait_until(lock, curr_time, []() { return m_wait_count == 0; });
    if (m_wait_count != 0) {
      std::cout << "Wait timeout\n";
    }
    std::string curr = std::to_string(std::stoi(count) - m_wait_count);
    std::string response = ":" + curr + "\r\n";
    send(client_fd, response.c_str(), response.length(), 0);
  }
