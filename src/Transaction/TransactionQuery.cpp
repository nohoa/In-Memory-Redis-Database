#include "TransactionQuery.hpp"
#include "../Map.h"
#include<mutex>
#include<chrono>

extern std::mutex mutex_guard ;

extern std ::unique_ptr<In_Memory_Storage> key_value_storage ;


extern long get_current_time_ms();

std::string Transaction_Query :: perform_incr(std:: string response , std::vector<std::string> parser_list){
    std::string key = parser_list[1];
        std::string value = key_value_storage->get(key,0);
        if(value == "") value = "0";
        bool digit = true;
        for(auto it : value){
            if(it >= '0' && it <= '9') continue ;
            else digit = false ;
        }
        if(digit == true){
        int current_value = std::stoi(value) +1 ;
        response = ":" + std::to_string(current_value)+"\r\n";
        mutex_guard.lock();
        key_value_storage->set(key,std::to_string(current_value),get_current_time_ms()+9999999999);
        mutex_guard.unlock();
        }
        
        else {
           response = "-ERR value is not an integer or out of range\r\n";
        }
        return response ;
}