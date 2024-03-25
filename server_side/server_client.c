
#include "server_client.h"

void initialize(int argc,char** argv,int * PORT,int* BACK_LOG,int* PART_SIZE){
    //it will initialize the variables based on the input given to the program
    if(argc==1){ 
        ERROR(true,"the min requirement to run the program is to determin the port(use -h for more info)");
    }
    for(int i=1;i<argc;i++){
        //port-------------------------
        if(!strcmp(argv[i],"-p")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -p");
            }else{
                *PORT=atoi(argv[i+1]);
                i++;
            }
        }
        //backlog---------------------
        if(!strcmp(argv[i],"-b")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -b");
            }else{
                *BACK_LOG=atoi(argv[i+1]);
                i++;
            }
        }
        //PART_SIZE
        if(!strcmp(argv[i],"-s")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -s");
            }else{
                *PART_SIZE=atoi(argv[i+1]);
                i++;
            }
        }
    }
}
struct serve* set_up_server(int PORT,int BACK_LOG){

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
