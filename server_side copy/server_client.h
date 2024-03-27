#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

#include "helper.h"

//structs------------------------------------
struct Client{
    struct sockaddr_in addr;
    int size_of_addr;
    bool accept_successful;
    char* IP[INET_ADDRSTRLEN];
    int socket_fd;
    char path[PATH_MAX];
    

};
struct Server{
    struct sockaddr_in addr;
    int socket_fd;
};

//functions----------------------------------------


void initialize(int argc,char** argv,int * PORT,int* BACK_LOG,char* BACK_UP_PORT,char* BACK_UP_IP,int* PART_SIZE);
struct serve* set_up_server(int PORT,int BACK_LOG);
struct Client* acceptClient(int server_socket);
bool handShake(struct Client* client,int PART_SIZE,const char* BACK_UP_PORT,const char* BACK_UP_IP);
void checkCommandAndExecute(char* command);

char* getAndSendCommandAndRecieveResult(struct Client* client,int PART_SIZE);

void checkAndPrintAnswer(char* answer);
bool sendMessage(struct Client* client,char* message,int PART_SIZE);
char* recieveMessage(struct Client* client,int PART_SIZE);
char* getNotEmptyWithOutBreakLineLine(struct Client* client);
void printInput(struct Client * client);
int exitCode(char* result);
bool CheckForSpecialCommands(char* command);
char* ExecuteForSpecialCommands(struct Client* client,char * command,int PART_SIZE);
char* ExecuteForOrdinaryCommands(struct Client* client,char * command,int PART_SIZE);

#endif