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
#include "threadpool.h"
#include "helper.h"


//structs------------------------------------
struct Client{
    struct sockaddr_in addr;
    int size_of_addr;
    bool accept_successful;
    char* IP[INET_ADDRSTRLEN];
    int socket_fd;
    char path[PATH_MAX];
    char* result;
    int id;
    bool deleted;
    

};
struct Server{
    struct sockaddr_in addr;
    int socket_fd;
};
struct Command{
    struct Client* client;
    char* command;
    int PART_SIZE;
};
struct KeepAcceptingInput{
    struct Server* server;
    int PART_SIZE;
    char BACK_UP_IP[20];
    char BACK_UP_PORT[10];
    
};
//functions----------------------------------------


void initialize(int argc,char** argv,int * PORT,int* BACK_LOG,char* BACK_UP_PORT,char* BACK_UP_IP,int* PART_SIZE,int* THREAD_NUM);
struct serve* set_up_server(int PORT,int BACK_LOG);
struct Client* acceptClient(int server_socket);
bool handShake(struct Client* client,int PART_SIZE,const char* BACK_UP_PORT,const char* BACK_UP_IP);
void checkCommandAndExecute(char* command);

char* getAndSendCommandAndRecieveResult(struct Client* client,int PART_SIZE);

void checkAndPrintAnswer(char* answer);
bool sendMessage(struct Client* client,char* message,int PART_SIZE,bool show_errors);
char* recieveMessage(struct Client* client,int PART_SIZE,bool show_errors);
char* getNotEmptyWithOutBreakLineLine(struct Client* client);
void printInput(struct Client * client);
int exitCode(char* result);
bool CheckForSpecialCommands(char* command);
char* ExecuteForSpecialCommands(struct Client* client,char * command,int PART_SIZE,bool show_errors);
char* ExecuteForOrdinaryCommands(struct Client* client,char * command,int PART_SIZE,bool show_errors);
void* keepAccepting(void * _input);
void* runIncommingCommands(void * _command );
struct Command* createCommand(struct Client* client,char* Command,int PART_SIZE);
bool checkCommandIsInternal(char* command);
bool executeInternalCommands(char* command,int* current_client);
bool CheckResultNeedingCommand(char* command);
void delete_client(struct Client* client);
bool checkForAggregateCommands(char* command);
void excuteAggregateCommand(char* command,struct ThreadPool* pool,int PART_SIZE);
#endif