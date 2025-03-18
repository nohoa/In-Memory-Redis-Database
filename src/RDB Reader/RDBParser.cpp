#include "RDBParser.hpp"
#include <string>
#include<sstream>
#include <chrono>


std::vector<std::vector<std::string> >  RDBParser :: read_path(std::string endpoint){

    std ::ifstream filestream(endpoint, std::ios::binary);
    std ::string s;
    std ::string all;
    std::vector<std::vector<std::string> > all_key_value;

    if(!filestream.is_open()) return all_key_value;
    
    while (getline(filestream, s)) {
      all += s;
    }
        std::vector<int> buf;
      std :: cout << all << std :: endl;
    for(int i = 0 ;i < all.size() ;i ++){
        buf.push_back(((htons((int)(all[i])) >> 8)&0xff));
    }

    int id = 0 ;
    while(id < buf.size() && buf[id] != 0xfe){
        id ++;
    }
    int no_key = buf[id+3];
    int no_exper = buf[id+4];
    id += 5;
    while(buf[id] == 0x00) id ++;
    //std :: cout << all[id] << std :: endl
    while(id < all.size() && (int)(all[id]) != -1){
    std :: string bin_key ;
    std::string bin_value ;
    std :: string experity = "";
    long current_time_in_ms;
    if(buf[id] == 0xfc){
      //std::cout << id <<std::endl;
        //std:: cout << "down here" << std::endl;
        unsigned char buff[8];
         for(int i = id+1 ;i <= id+8 ;i ++){
          buff[i-id-1] = all[i];
          //std :: cout << ss.str() << std::endl;
          }

            auto expiry_ms = (uint64_t)buff[0]
                            | ((uint64_t)buff[1] << 8)
                            | ((uint64_t)buff[2] << 16)
                            | ((uint64_t)buff[3] << 24)
                            | ((uint64_t)buff[4] << 32)
                            | ((uint64_t)buff[5] << 40)
                            | ((uint64_t)buff[6] << 48)
                            | ((uint64_t)buff[7] << 56);
            auto exper = std::chrono::system_clock::time_point(std::chrono::milliseconds(expiry_ms));
            auto now_in_ms =
            std::chrono::time_point_cast<std::chrono::milliseconds>(exper);
            auto value = now_in_ms.time_since_epoch();
            current_time_in_ms = value.count();
        for(int i = id+1 ;i <= id+8 ;i ++){
          std::stringstream ss ;
          ss << buf[i];
          //std :: cout << ss.str() << std::endl;
          experity +=  ss.str();
        }
        id += 9;
    } 
    while(buf[id] == 0x00) id ++;
    if(all[id] >='a' && all[id] <='z'){
      while(all[id] >='a' && all[id] <= 'z'){ 
        bin_key += all[id];
        id ++;
      }
    }
    else  {
    int sz = (int)(all[id]);
    id ++;
    while (sz > 0) {
      bin_key += all[id];
      id++;
      sz--;
      }
    }
    
    if(all[id]  >= 'a' && all[id] <='z'){
      while(all[id] >='a' && all[id] <='z'){
        bin_value += all[id];
          id ++;
      }

    }
    else {
    int value_sz = (int)(all[id]);
    id++;
    while (value_sz > 0) {
      bin_value += all[id];
      id++;
      value_sz--;
      }
    }
    // uint64_t unixTime;
    // std::stringstream ss1(experity);
    // ss1 >> unixTime;  // Convert string to integer
    // // std :: cout << "experity is " << current_time_in_ms << std::endl;
    // // std ::cout <<  " key is " << bin_key << std ::endl;
    // // std ::cout <<  " value is " << bin_value << std ::endl;
    std::vector<std::string > merge ;
    merge.push_back(bin_key);
    merge.push_back(bin_value);
    if(!experity.empty())merge.push_back(std::to_string(current_time_in_ms));
    else merge.push_back("-1");
    all_key_value.push_back(merge);
    no_key --;
    if(no_key== 0) break;
    //id ++;
    }
    return all_key_value;
}