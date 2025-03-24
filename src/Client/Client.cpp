 #include "Client.hpp"


 int  Client_Request :: send_request (std:: string& port, std:: string&replica_no, struct sockaddr_in& server_addr ){

 std :: string port_no = "";
    int id = port.size()-1;
    while(port[id]  >= '0' && port[id] <= '9'){
      port_no += port[id];
      id --;
    }
    std::reverse(port_no.begin(),port_no.end());
   // std :: cout << port_no << std::endl ;
    //std :: cout << port << std::endl;
    struct sockaddr_in back_up_addr;
    back_up_addr.sin_family = AF_INET;
    back_up_addr.sin_addr.s_addr = INADDR_ANY;
    back_up_addr.sin_port = htons(std::stoi(port_no));
    int backup_fd ;
    if((backup_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
       std :: cout << "Error  socket creation" << std :: endl;
       return - 1 ;
    }
    std :: cout << "Connecting to master " << std:: endl;

    int status ;

     if ((status
         = connect(backup_fd, (struct sockaddr*)&back_up_addr,
                   sizeof(back_up_addr)))
        < 0) {
        printf("\nConnection Failed \n");
    }
    else {
    std :: string SEND_PING  = "*1\r\n$4\r\nPING\r\n";
    send(backup_fd,SEND_PING.c_str(),SEND_PING.length(),0);
    char msg[1024];
    while( recv(backup_fd, &msg, sizeof(msg), 0) <= 0 ) ;

    std :: string SEND_REPLCONF = "*3\r\n$8\r\nREPLCONF\r\n$14\r\nlistening-port\r\n$4\r\n6380\r\n";
    send(backup_fd,SEND_REPLCONF.c_str(),SEND_REPLCONF.length(),0);
    while( recv(backup_fd, &msg, sizeof(msg), 0) <= 0 ) ;
    SEND_REPLCONF = "*3\r\n$8\r\nREPLCONF\r\n$4\r\ncapa\r\n$6\r\npsync2\r\n";
    send(backup_fd,SEND_REPLCONF.c_str(),SEND_REPLCONF.length(),0);

    while( recv(backup_fd, &msg, sizeof(msg), 0) <= 0 ) ;

    std :: string synch = "*3\r\n$5\r\nPSYNC\r\n$1\r\n?\r\n$2\r\n-1\r\n";
    send(backup_fd,synch.c_str(),synch.length(),0);
    
    }
    server_addr.sin_port = htons(std::stoi(replica_no)) ;

    return 0 ;

 }