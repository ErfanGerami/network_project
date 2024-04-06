#define _POSIX_C_SOURCE 200809L

#include "server_client.h"


#include "helper.h"
int PORT=3000;
int PART_SIZE=1024;
int BACK_LOG=3;
char BACK_UP_PORT[20]="UNSET";//set to 0 for unset
char BACK_UP_IP[20]="NOTSET";//set to UNSET for unset
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
    signal(SIGPIPE, SIG_IGN);//this line is for when the client closed connection 
    //and we tried to read or write to the socket the signal to tereminate our program 
    //wont close it instead the error will be printed using perror

    //my variables------------------------------------------
    int result;
    char command[400];
    //-------------------------------------------------
    initialize(argc,argv,&PORT,&BACK_LOG,&BACK_UP_PORT,&BACK_UP_IP,&PART_SIZE);
    struct Server* server=set_up_server(PORT,BACK_LOG);
    while(1){
        printf("waiting for clients to join\n   ");
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
            //getting command and answer in while loop
            char* answer=getAndSendCommandAndRecieveResult(client,PART_SIZE);
            
            if(!answer){//if the answer is not valid(be NULL) the connection is broken so we wait for the next one
                close(client->socket_fd);
                break;
            }
         
            checkAndPrintAnswer(answer);

            free(answer);
        }
        free(client);

    }

}

