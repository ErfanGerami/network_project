#define _POSIX_C_SOURCE 200809L

#include "server_client.h"
#include "helper.h"
int PORT=3000;
int PART_SIZE=1024;
int BACK_LOG=3;
char* BACK_UP_PORT="NOTSET";
char* BACK_UP_IP="NOTSET";
/*
protocole works like this:
in the beginning of the connection server sends some data like PART_SIZE BACK_UP_PORT BACK_UP_IP
then it waites for a command when a command is typed by servers user it send it to the client in parts
which their max lengh is the PART_SIZE
then waites for the client to say how much data client wants to send 
then recieves that much data( so we dont have to add a special char to know the end of data because
it could raise some bugs for example having the same characters in the main data)
client add some headers like 1 if the command was successfull so it never returns nothing
*/
int main(int argc,char** argv){
    //my variables------------------------------------------
    int result;
    char command[400];
    //-------------------------------------------------
    initialize(argc,argv,&PORT,&BACK_LOG,&BACK_UP_PORT,&BACK_UP_IP,&PART_SIZE);
    struct Server* server=set_up_server(PORT,BACK_LOG);
    while(1){

        struct Client* client=acceptClient(server->socket_fd);
        if(!client->accept_successful){
            free(client);//the memory is dynamicly allocated so it shoul be deallocated
            continue;
        }

        printf("a connection were made\n");
        if(!handShake(client,PART_SIZE,BACK_UP_IP,BACK_UP_PORT)){
            continue;
        }
        while(true){
            char* answer=getAndSendCommandAndRecieveResult(client,PART_SIZE);
            checkAndPrintAnswer(answer);
            free(answer);




        }

    }

}

