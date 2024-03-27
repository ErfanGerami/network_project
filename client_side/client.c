#include "client_server.h"


int PORT=3000;
char* IP="127.0.0.1";
int BACK_UP_PORT=0;
char* BACK_UP_IP="UNSET";
int PART_SIZE;
const int MAX_TRY=5;
char path[PATH_MAX];

int main(int argc,char** argv){
    PORT=atoi(argv[1]);
    int try_cnt=0;
    struct Client* client;
    while(!(client=set_up_client(PORT,IP))&& try_cnt<MAX_TRY){
        try_cnt++;
        printf("not connected.\n");
        printf("trying again...\n");
    }
    if(try_cnt>=MAX_TRY){
        ERROR(true,"failed to connect");
    }
    handShake(client,&PART_SIZE,&BACK_UP_PORT,BACK_UP_IP);

    while(true){

        if(!getAndRunCommand(client,PART_SIZE)){
            return 1;
        }
    }
    close(client->socket_fd);


}
