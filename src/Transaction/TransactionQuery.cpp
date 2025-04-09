#include "TransactionQuery.hpp"
#include "../Map.h"
#include <chrono>
#include <mutex>
#include <unistd.h>

extern std::mutex mutex_guard;

extern std ::unique_ptr<In_Memory_Storage> key_value_storage;

extern bool inside;
extern std::vector<int> replica_id;

extern long get_current_time_ms();

std::string
Transaction_Query ::perform_incr(std::string response,
                                 std::vector<std::string> parser_list) {
  std::string key = parser_list[1];
  std::string value = key_value_storage->get(key, 0);
  if (value == "")
    value = "0";
  bool digit = true;
  for (auto it : value) {
    if (it >= '0' && it <= '9')
      continue;
    else
      digit = false;
  }
  if (digit == true) {
    int current_value = std::stoi(value) + 1;
    response = ":" + std::to_string(current_value) + "\r\n";
    mutex_guard.lock();
    key_value_storage->set(key, std::to_string(current_value),
                           get_current_time_ms() + 9999999999);
    mutex_guard.unlock();
  }

  else {
    response = "-ERR value is not an integer or out of range\r\n";
  }
  return response;
}

std::string Transaction_Query::perform_set(std::string response,
                                           std::vector<std::string> parser_list,
                                           std::vector<std::string> all_cmd) {
  mutex_guard.lock();
  inside = true;
  long current_time_in_ms = get_current_time_ms();

  response = "+OK\r\n";
  if (parser_list.size() > 3)
    key_value_storage->set(parser_list[1], parser_list[2],
                           current_time_in_ms + stoi(parser_list.back()));
  else
    key_value_storage->set(parser_list[1], parser_list[2],
                           current_time_in_ms + 999999999999);

  mutex_guard.unlock();
  std ::string set_response = "*";
  set_response += std::to_string(all_cmd.size());
  set_response += "\r\n";
  for (auto it : all_cmd) {
    set_response += "$";
    set_response += std::to_string(it.size());
    set_response += "\r\n";
    set_response += it;
    set_response += "\r\n";
  }
  for (int i = 0; i < replica_id.size(); i++) {

    write(replica_id[i], set_response.c_str(), set_response.size());
  }
  return response;
}

std::string
Transaction_Query ::perform_get(std::string response,
                                std::vector<std::string> parser_list) {

  mutex_guard.lock();
  long current_time_in_ms = get_current_time_ms();
  response = key_value_storage->get(parser_list[1], current_time_in_ms);

  mutex_guard.unlock();
  if (response == "") {
    response = "$-1\r\n";
  } else {
    response =
        "$" + std::to_string(response.size()) + "\r\n" + response + "\r\n";
  }
  return response;
}