#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int PORT;//-p
int BACK_LOG;//-b
int PART_SIZE;//-s
char buffer[1024];
char temp[300];//just for use in some cases
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
void changeColor(const char* color){
    printf(color);
}
void ERROR(bool _exit,const char* message){
     if(message!=NULL){  
        strcpy(temp,RED);
        strcat(temp,message);
        perror(temp);
    }else{
        strcpy(temp,RED);
        strcat(temp,"ERROR");
        perror(temp);
    }
    changeColor(RESET);
    if(_exit){
        exit(1);//exiting with non zero valu
    }

}

void initialize(int argc,char** argv){
    
    if(argc==1){ 
        ERROR(true,"the min requirement to run the program is to determin the port(use -h for more info)");
    }
    for(int i=1;i<argc;i++){
        //port-------------------------
        if(!strcmp(argv[i],"-p")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -p");
            }else{
                PORT=atoi(argv[i+1]);
                i++;
            }
        }
        //backlog---------------------
        if(!strcmp(argv[i],"-b")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -b");
            }else{
                BACK_LOG=atoi(argv[i+1]);
                i++;
            }
        }
        //PART_SIZE
        if(!strcmp(argv[i],"-s")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -s");
            }else{
                PART_SIZE=atoi(argv[i+1]);
                i++;
            }
        }
    }
}
struct serve* set_up_server(){

    /*
    run bind and listen then returns the server info
    */
    int result;


    struct Server* server=(struct Server*)malloc(sizeof(struct Server));
    server->socket_fd=socket(AF_INET,SOCK_STREAM,0);//creating socket
    //checking for error
    if(server->socket_fd==-1){
        ERROR(true,NULL); 
    }
    printf("socket created successfully\n");
    

    server->addr.sin_addr.s_addr=INADDR_ANY;
    server->addr.sin_family=AF_INET;
    server->addr.sin_port=htons(PORT);
    result=bind(server->socket_fd,&(server->addr),sizeof(struct sockaddr_in));
    if(result<0){

        ERROR(true,NULL);

    }
    printf("server bound successfully\n");

    result=listen(server->socket_fd,BACK_LOG);
    if(result<0){
        ERROR(true,NULL);

    }
    printf("server is listening\n");
    return server;

}
struct Client* acceptClient(int server_socket){
    /*
    accepts a client and returns its data
    */
   struct Client* client=(struct Client*)malloc(sizeof(struct Client));
   client->accept_successful=true;
   client->size_of_addr=sizeof(client->addr);
   client->socket_fd=accept(server_socket,&(client->addr),&(client->size_of_addr));
   
    if(client->socket_fd==-1){  
        ERROR(false,"invalid connection");
        client->accept_successful=false;

    }
    inet_ntop(AF_INET,&(client->addr.sin_addr),client->IP,INET_ADDRSTRLEN);

    return client;

}
int main(int argc,char** argv){
    //my variables------------------------------------------
    int result;
    char client_IP[INET_ADDRSTRLEN];
    //-------------------------------------------------
    initialize(argc,argv);
    struct Server* server=set_up_server();
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