#ifndef MAP_H
#define MAP_H

#include<iostream>
#include<map>
#include <string>
#include <utility>
#include<vector>

class In_Memory_Storage {
    private :

    std :: map<std::string , std :: pair<std :: string , long long > > storage ;

    std::map<std::string, std::string> type ;

    std::map<std::string,int> sequence_no ; 

    std::map<std::pair<std::string,std::string>,std::pair<std::string,std::string> > stream;

    public : 


    virtual void set(std :: string key , std :: string value , long long expiration_time );

    virtual std::string get (std :: string key, long long current_time) ;

    virtual void clean (long long current_time );

    virtual bool exist(std::string key);

    virtual bool exist_type(std::string key);

    virtual std::vector<std::string> getAllKey();

    virtual void set_type(std:: string key,std::string type);

    virtual std::string get_type(std:: string key);

    virtual void set_seq(std::string sequence_no);

    virtual std::vector<std::string> get_all_seq();

    virtual void set_stream(std::pair<std::string , std::string> key,std::pair<std::string , std::string> value );
    virtual std::pair<std::string , std::string> get_stream(std::pair<std::string , std::string>key);

    std::vector<std::vector<std::string> > get_range(std::string left , std::string right);

};


#endif