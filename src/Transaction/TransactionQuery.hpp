#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include<string>

class Transaction_Query {

    public  : 
    Transaction_Query(){};
    ~Transaction_Query(){};
    virtual std::string perform_incr(std:: string response , std::vector<std::string> parser_list) ;

    
};

#endif 