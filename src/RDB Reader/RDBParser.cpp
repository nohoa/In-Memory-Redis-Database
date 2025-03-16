#include "RDBParser.hpp"


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

    id += 4;
    while(buf[id] == 0x00) id ++;
    //std :: cout << all[id] << std :: endl
    while(id < all.size() && (int)(all[id]) != -1){
    std :: string bin_key ;
    std::string bin_value ;
    if(all[id] >='a' && all[id] <='z'){
      while(all[id] >='a' && all[id] <= 'z'){ 
        bin_key += all[id];
        id ++;
      }
    }
    else {
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
    std ::cout <<  " key is " << bin_key << std ::endl;
    std ::cout <<  " value is " << bin_value << std ::endl;
    std::vector<std::string > merge ;
    merge.push_back(bin_key);
    merge.push_back(bin_value);
    all_key_value.push_back(merge);
    no_key --;
    if(no_key== 0) break;
    id ++;
    }
    return all_key_value;
}