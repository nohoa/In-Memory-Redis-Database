#ifndef MAP_H
#define MAP_H

#include<iostream>
#include<map>
#include<vector>

class In_Memory_Storage {
    private :

    std :: map<std::string , std :: pair<std :: string , long long > > storage ;

    public : 


    virtual void set(std :: string key , std :: string value , long long expiration_time );

    virtual std::string get (std :: string key, long long current_time) ;

    virtual void clean (long long current_time );

    virtual bool exist(std::string key);

    virtual std::vector<std::string> getAllKey();

};


#endif