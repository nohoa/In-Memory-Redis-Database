
#include "Map.h"
#include <ctime>
#include <vector>

void In_Memory_Storage :: set(std::string key, std::string value, long long expiration_time ) {
        storage[key] = make_pair(value,expiration_time) ;
}

std::string In_Memory_Storage :: get(std::string key,long long expiration_time) {
        clean(expiration_time);
        
       // std :: cout << "current is " << storage.size() << std :: endl;
       // std :: cout << "key is " << key << std :: endl;
        // for(auto it : storage) {
        //     std :: cout << it.first << " " << it.second.first << std :: endl;
        // }
        if(!storage.count(key)){
                return "";
        }
        return storage[key].first;
}

void In_Memory_Storage :: clean(long long current_time ){
    std :: map<std::string , std :: pair<std :: string , long long > > tmp ;
   // std :: cout << current_time << std :: endl;
   // std :: cout << "Size before " << storage.size() << std :: endl; 
    //std :: cout << current_time << std::endl;
    for(auto it : storage){
        //std :: cout << it.second.second << std::endl;
        if(it.second.second  >= current_time){
            tmp[it.first] = it.second;
        }
    }
   storage.clear();
   for(auto it : tmp){
        storage[it.first] = it.second;
   }
   //std :: cout << "Size after " << storage.size() << std :: endl; 
   tmp.clear();
}

std::vector<std::string> In_Memory_Storage :: getAllKey(){
    std::vector<std::string> keys ;
    for(auto it : storage){
        if(it.first != "dir" && it.first != "dbfilename") keys.push_back(it.first);
    }
    return keys;
}