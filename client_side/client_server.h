#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>
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
    char path[PATH_MAX];
    pid_t child_pid;
    
};
void initialize(int argc,char** argv,int * PORT,char* IP,bool* DETACH);

struct Client*  set_up_client(int PORT,char* IP);
bool handShake(struct Client* client,int* PART_SIZE,int* BACK_UP_PORT, char* BACK_UP_IP);
char* runCommand(char* command);
bool getAndRunCommand(struct Client* client,int PART_SIZE);
bool sendMessage(struct Client* client,char* message,int PART_SIZE);
char* recieveMessage(struct Client* client,int PART_SIZE);
char* CheckForSpecialCommands(struct Client* path,char * command);
void fillResultWithExitCode(char* result,int exit_code);
#endif
