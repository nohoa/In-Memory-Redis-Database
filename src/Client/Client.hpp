#ifndef CLIENT_H
#define CLIENT_H

#include<string>
#include <sys/socket.h>
#include <sys/types.h>
#include<iostream>
#include <arpa/inet.h>
#include<algorithm>
#include<vector>

class Client_Request {
    public :
    Client_Request(){};
    ~Client_Request(){};
   virtual int send_request(std:: string& port,std:: string&replica_no,struct sockaddr_in& server_add , int server_fd, int argc, char**argv) ;
   //virtual int send_request1(std:: string& port,std:: string&replica_no,struct sockaddr_in& server_add ) ;


};

#endif