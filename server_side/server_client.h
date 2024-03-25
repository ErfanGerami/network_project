#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

//structs------------------------------------
struct Client{
    struct sockaddr_in addr;
    int size_of_addr;
    bool accept_successful;
    char* IP[INET_ADDRSTRLEN];
    int socket_fd;
    

};
struct Server{
    struct sockaddr_in addr;
    int socket_fd;
};

//functions----------------------------------------


void initialize(int argc,char** argv,int * PORT,int* BACK_LOG,int* PART_SIZE);
struct serve* set_up_server(int PORT,int BACK_LOG);
struct Client* acceptClient(int server_socket);

#endif