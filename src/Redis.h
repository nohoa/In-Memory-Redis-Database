#ifndef REDIS_H
#define REDIS_H

#include<iostream>
#include<vector>
#include<map>



class Redis {
   private :
   std :: string response ;

    public :

     Redis(std ::string current_response) 
    : response(current_response){} 

    ~Redis(){};
    
    virtual std :: vector<std :: string> get_command(std :: string response);

    virtual std :: vector<std::string > get_all_command(std :: string response);

    virtual std :: vector<std::string > get_client_command(std :: string response);

}; 

#endif