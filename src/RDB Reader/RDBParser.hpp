#ifndef RDB_H
#define RDB_H

#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include <arpa/inet.h>

class RDBParser {
    public :
    RDBParser(){}
    ~RDBParser(){};
    virtual std::vector<std::vector<std::string> > read_path(std::string endpoint);
} ;


#endif