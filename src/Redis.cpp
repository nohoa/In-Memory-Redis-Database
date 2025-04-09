#include "Redis.h"

std::vector<std::string> Redis ::get_command(std::string response) {
  std::vector<std::string> v;

  std::string running = "";
  int id = 0;
  while (id < response.size()) {
   
    if (response[id] == '\r' || response[id] == '\n') {
      if (running.size() > 0)
        v.push_back(running);
      running.clear();
    } else {
      running += response[id];
    }
    id++;
  }
  std ::vector<std::string> parser;

  if (v[0][0] == '*') {
    for (int i = 2; i < v.size(); i += 2) {
      parser.push_back(v[i]);
    }
  } else {
    for (int i = 1; i < v.size(); i += 2) {
      parser.push_back(v[i]);
    }
  }
  return parser;
}

std::vector<std::string> Redis ::get_all_command(std ::string response) {
  std::vector<std::string> v;
  
  std::string running = "";
  int id = 0;
  while (id < response.size()) {
   
    if (response[id] == '\r' || response[id] == '\n') {
      if (running.size() > 0)
        v.push_back(running);
      running.clear();
    } else {
      running += response[id];
    }
    id++;
  }
  std ::vector<std::string> parser;

  for (int i = 1; i < v.size(); i++) {
    parser.push_back(v[i]);
  }
  return parser;
}

std::vector<std::string> Redis ::get_client_command(std::string response) {
        std::vector<std::string> v;
        
        std::string running = "";
        int id = 0;
        while (id < response.size()) {
          
          if (response[id] == '\r' || response[id] == '\n') {
            if (running.size() > 0)
              v.push_back(running);
            running.clear();
          } else {
            running += response[id];
          }
          id++;
        }
        std ::vector<std::string> parser;
      
          for (int i = 2; i < v.size(); i += 2) {
            parser.push_back(v[i]);
          }

        return parser;
      }