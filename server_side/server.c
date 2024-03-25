#include <stdio.h>

#include "server_client.h"
#include "helper.h"
int PORT=3000;
int PART_SIZE=1024;
int BACK_LOG=3;
int main(int argc,char** argv){
    //my variables------------------------------------------
    int result;
    //-------------------------------------------------
    initialize(argc,argv,&PORT,&BACK_LOG,&PART_SIZE);
    struct Server* server=set_up_server(PORT,BACK_LOG);
    while(1){

        struct Client* client=acceptClient(server->socket_fd);
        if(!client->accept_successful){
            free(client);//the memory is dynamicly allocated so it shoul be deallocated
        }

        printf("a connection were made\n");
        while(true){
            printf("%s$ ",client->IP);
            scanf("%s",1);
            

        }

    }











}