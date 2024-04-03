#include <signal.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <unistd.h>
#include <stdlib.h>


#include "client_server.h"


int PORT=3003;
char IP[20]="127.0.0.1";
int BACK_UP_PORT=0;
char BACK_UP_IP[20]="UNSET";
int PART_SIZE;
const int MAX_TRY=5;
char path[PATH_MAX];
void childSignalHandler(int signo) {
    if (signo == SIGUSR1) {
        printf("child process running now\n");
    }
}
int main(int argc,char** argv){
    //PORT=atoi(argv[1]);
    //IP=argv[1];
    initialize(argc,argv,&PORT,IP);

    struct sigaction sa;
    sa.sa_handler = childSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    


    int try_cnt=0;
    struct Client* client;
    while(!(client=set_up_client(PORT,IP))&& try_cnt<MAX_TRY){
        try_cnt++;
	    printf("trying to connect to %s:%d\n",IP,PORT);
        printf("trying again...\n");
    }
    if(try_cnt>=MAX_TRY){
        ERROR(true,"failed to connect");
    }
    handShake(client,&PART_SIZE,&BACK_UP_PORT,BACK_UP_IP);
    if(client->child_pid==0){//it means we are the child
        printf("child process created...\n");
        signal(SIGUSR1, childSignalHandler);
        strcpy(IP,BACK_UP_IP);
        PORT=BACK_UP_PORT;
        pause();  
        printf("child process unpaused\n");  
        printf("PART_SIZE:%d BACK_UP_PORT:%d BACK_UP_IP:%s\n",PART_SIZE,BACK_UP_PORT,BACK_UP_IP);
         while(!(client=set_up_client(PORT,IP))&& try_cnt<MAX_TRY){
            try_cnt++;
            printf("trying to connect to %s:%d\n",IP,PORT);
            printf("trying again...\n");
        }
        if(try_cnt>=MAX_TRY){
            ERROR(true,"failed to connect");
        }
        handShake(client,&PART_SIZE,&BACK_UP_PORT,BACK_UP_IP);



    }
    while(true){
        printf("%d\n",client->child_pid);
            
        if(!getAndRunCommand(client,PART_SIZE)){
            printf("%d\n",client->child_pid);
            printf("closing parent process\n");
            kill(client->child_pid, SIGUSR1);
            close(client->socket_fd);   
            return 1;
        }
    }
    


}
