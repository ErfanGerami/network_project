
#include "server_client.h"

void initialize(int argc,char** argv,int * PORT,int* BACK_LOG,char* BACK_UP_PORT,char* BACK_UP_IP,int* PART_SIZE){
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

        if(!strcmp(argv[i],"-bp")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -bp");
            }else{
                strcpy(BACK_UP_PORT,argv[i+1]);
                i++;
            }
        }
        if(!strcmp(argv[i],"-bi")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -bi");
            }else{
                strcpy(BACK_UP_IP,argv[i+1]);
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

bool handShake(struct Client* client,int PART_SIZE,const char* BACK_UP_PORT,const char* BACK_UP_IP){


    char message[200]={0};
 
    sprintf(message,"%d %s %s",PART_SIZE,BACK_UP_IP,BACK_UP_PORT);

    if(send(client->socket_fd,message,strlen(message),0)<=0){
        ERROR(false,"handshake unsuccessfull");
        return false;
    }
    return true;
   
}
char* getAndSendCommandAndRecieveResult(struct Client* client,int PART_SIZE){
    char* command=NULL;
    changeColor(BLUE);
    printf("%s$ ",client->IP);
    
    changeColor(RESET);
    size_t line=0;/*I dont know why but making this int will make the socket not accepting stuff I 
    I even ran a simple server socket and it did the same.I dont know why should it effect the socket but 
    it is c after all*/

    int size_of_command=getline(&command,&line,stdin)-1;
    
    command[size_of_command]='\0';
    
    
    int sent=0;
    
    //it sends data in parts until all the data is send
    while(sent<size_of_command){
        int data_to_send=min(PART_SIZE,size_of_command-sent);
        if(send(client->socket_fd,command+sent,data_to_send,0)<data_to_send){
            ERROR(false,NULL);
            return NULL;
        }
        sent+=data_to_send;
    }
    char* answer_size_string[20]={0};
    if(recv(client->socket_fd,answer_size_string,sizeof(answer_size_string),0)<=0){
            ERROR(false,NULL);
            return NULL;
    }
    
    int recieving_data_size=atoi(answer_size_string);
    char* answer=(char*)calloc(recieving_data_size+3,sizeof(char));
    
    int recieved=0;
    while(recieved<recieving_data_size){
        int data_to_recieve=min(PART_SIZE,recieving_data_size-recieved);
        if(recv(client->socket_fd,answer+recieved,data_to_recieve,0)<0){
            ERROR(false,NULL);
            return NULL;
        }
        recieved+=data_to_recieve;
    }
    return answer;
    


}

void checkAndPrintAnswer(char* answer){
    if(answer[0]=='1'){
        changeColor(BLUE);
    }else{
        changeColor(RED);
    }
    printf(answer+sizeof(char));
    changeColor(RESET);
}

