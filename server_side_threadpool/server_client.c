
#include "server_client.h"
#define  MAX_CLIENTS 60

struct Client* clients[MAX_CLIENTS]={NULL};//max is set to be 60
int client_cnt=0;
pthread_rwlock_t lock_for_clients;
pthread_cond_t condition_var_for_clients;
void printInput(struct Client * client){
    //prints client ip along with the path 
    if(client)
        printf("(%d),%s@:%s$ ",client->id,client->IP,client->path);
    else
        printf("Internalcommands only:");

}
bool sendMessage(struct Client* client,char* message,int PART_SIZE,bool show_errors){
    //sends the lenght of the message in 20 chars so other side knows how much should it read 
    //then sends the actual data  and if everything goes as expected it returns true otherwise false
    char size_of_result[21]={0};
    sprintf(size_of_result,"%d",strlen(message));
    //the size of number should be exactly 20
    //because we have two sends in continous we should specifiy their size

    if(send(client->socket_fd,size_of_result,20,0)<=0){
        if(show_errors)
            ERROR(false,"error while sending size");
        return false;
    }
    int size_of_message=strlen(message);
    int sent=0;

    //it sends data in parts until all the data is send
    while(sent<size_of_message){
        int data_to_send=min(PART_SIZE,size_of_message-sent);
        if(send(client->socket_fd,message+sent,data_to_send,0)<data_to_send){
            if(show_errors)
                ERROR(false,NULL);
            return false;
        }
        sent+=data_to_send;
    }

    return true;

}
char* recieveMessage(struct Client* client,int PART_SIZE,bool show_errors){
    //firsts recieve the size of data in 20 chars and then recieves the actual data and return it 
    //if anything goes wrong it will return NULL
    char* answer_size_string[21]={0};
    if(recv(client->socket_fd,answer_size_string,20,0)<=0){//I read exacly 20 
        if(show_errors)
            ERROR(false,NULL);
        return NULL;
    }
    int recieving_data_size=atoi(answer_size_string);
    char* answer=(char*)calloc(recieving_data_size+3,sizeof(char));
    int recieved=0;
    
    while(recieved<recieving_data_size){
        int data_to_recieve=min(PART_SIZE,recieving_data_size-recieved);
        if(recv(client->socket_fd,answer+recieved,data_to_recieve,0)<=0){
            if(show_errors)
                ERROR(false,NULL);
            return NULL;
        }
        recieved+=data_to_recieve;
    }
    return answer;
}
char* getNotEmptyWithOutBreakLineLine(struct Client* client){
    printInput(client);
    char* command=NULL;
     size_t line=0;/*I dont know why but making this int will make the socket not accepting stuff I 
    I even ran a simple server socket and it did the same.I dont know why should it effect the socket but 
    it is c after all*/
    int size_of_command;
    while (!(size_of_command=getline(&command,&line,stdin)-1)){//getlines until line is not empty   
        changeColor(BLUE);
        printInput(client);
        changeColor(RESET);
    }
    
    command[size_of_command]='\0';
    return command;

}
void initialize(int argc,char** argv,int * PORT,int* BACK_LOG,char* BACK_UP_PORT,char* BACK_UP_IP,int* PART_SIZE,int* THREAD_NUM){
    //it will initialize the variables based on the input given to the program
    /*if(argc==1){ 
        ERROR(true,"the min requirement to run the program is to determin the port(use -h for more info)");
    }*/
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
        if(!strcmp(argv[i],"-t")){
            if(argc==i+1){//it means nothing is after that
                ERROR(true,"nothing came after -t");
            }else{
                *THREAD_NUM=atoi(argv[i+1]);
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
    
    
    //first server sends PART_SIZE BACK_UP_IP BACK_UP_PORT
    //then client sends its path
    char message[401]={0};
 
    sprintf(message,"%d %s %s",PART_SIZE,BACK_UP_IP,BACK_UP_PORT);

    if(!sendMessage(client,message,400,true)){
        ERROR(false,"handshake unsuccessfull");
        return false;
    }
    char * temp_path;
    if(!(temp_path=recieveMessage(client,PART_SIZE,true))){
        ERROR(false,"error while handshaking");
        return false;
    }
    strcpy(client->path,temp_path);
    free(temp_path);

    
    return true;
   
}
char* getAndSendCommandAndRecieveResult(struct Client* client,int PART_SIZE){

    changeColor(BLUE);
    printInput(client);
    changeColor(RESET);
   
    
    char* command=getNotEmptyWithOutBreakLineLine(client);
    
    char* result;
    
    //it could return NULL when SendMessage and RecieveMessage return 
    if((CheckForSpecialCommands(command))){
        result= ExecuteForSpecialCommands(client,command,PART_SIZE,false);
        free(command);
        return result;

    }else{
        result= ExecuteForOrdinaryCommands(client,command,PART_SIZE,false);
        free(command);
        return result;
    }

}


void checkAndPrintAnswer(char* answer){
    /*
    checks the exit code of the result sent by client and prints it accordingly
    */

    int exit_code=(answer[0]-'0')*100+(answer[1]-'0')*10+answer[2]-'0';
    if(!exit_code){
        changeColor(PURPLE);
    }else{
        changeColor(RED);
    }
    printf("%s\n",answer+3);
    changeColor(RESET);
}
int exitCode(char* result){
    //extracts exit code from the result sent by client
    return (result[0]-'0')*100+(result[1]-'0')*10+(result[2]-'0');
}
bool CheckForSpecialCommands(char* command){
    //checks if the command is not a sual terminal command and instead a command that should be treated specially
    char* temp=(char*)malloc(sizeof(char)*(strlen(command)+2));
    strcpy(temp,command);
    while(*temp==' '){
        temp++;
    }
    char* sp;
    while((sp=strchr(temp,' '))){
        *sp=0;
    
    }

    
    if(!strcmp(temp,"cd")||!strcmp(temp,"get_file")||!strcmp(temp,"send_file")||!strcmp(temp,"sendall")){
        return true;
        free(temp);
    }
    return false;
}
char* ExecuteForSpecialCommands(struct Client* client,char * command,int PART_SIZE,bool show_errors){
    //execute some commands that should be treated diffrently 
    char* result=NULL;
    char* temp=(char*)malloc(sizeof(char)*(strlen(command)+2));
    strcpy(temp,command);
    char* end_of_command=temp+strlen(temp);
    while(*temp==' '){
        temp++;
    }
    char* sp;
    while((sp=strchr(temp,' '))){
        *sp=0;
    }
    if(!strcmp(temp,"sendall")){
        char* next_part=nexPart(temp,end_of_command);
        if(!next_part){
            if(show_errors){
                ERROR(false,"not valid command");
            }
            return NULL;
        }
        
        for(int i=0;i<MAX_CLIENTS;i++){
            pthread_mutex_lock(&lock_for_clients);

            if(clients[i] && !clients[i]->deleted){
                ExecuteForSpecialCommands(clients[i],command+(next_part-temp),PART_SIZE,show_errors);
            }
            pthread_mutex_unlock(&lock_for_clients);

        }
        char* result=(char*)malloc(sizeof(char)*(5));
        strcpy(result,"000");
        return result;


    }

    if(!strcmp(temp,"cd")){
        
        if(!sendMessage(client,command,PART_SIZE,show_errors)){
            return NULL;
        }
        result=recieveMessage(client,PART_SIZE,show_errors);
        if(!result){
            return NULL;
        }
        if(exitCode(result)==0){
            
            strcpy(client->path,result+3);
        }
       
    }
    if(!strcmp(temp,"get_file")){
        
        if(!sendMessage(client,command,PART_SIZE,show_errors)){
            return NULL;
        }
        
        result=recieveMessage(client,PART_SIZE,show_errors);
        if(!result){
            return NULL;
        }
        FILE* file=fopen(temp+strlen(temp)+1,"w");
        if(!file){
            //ERROR(false,"couldnt open file in server side");
            return NULL;
        }
        else{
            fprintf(file,result+3);
            fclose(file);   
        }
        
    }
    if(!strcmp(temp,"send_file")){
        char* temp_command=malloc(sizeof(char)*(101+strlen(command)));
        strcpy(temp_command,command);
        strcat(temp_command," ");
        FILE* file=fopen(temp+strlen(temp)+1,"r");

        if(!file){

            ERROR(false,"problem opening file on server side");
        }
        else{
            char temp[100]={0};
            int curr_size=101+strlen(temp_command);
            while(fgets(temp,sizeof(temp),file)){
                curr_size+=101;
                temp_command=(char*)realloc(temp_command,curr_size*sizeof(char*));
                strcat(temp_command,temp);
            }
            fclose(file);   
        }


        if(!sendMessage(client,temp_command,PART_SIZE,show_errors)){
            return NULL;
        }
        
        result=recieveMessage(client,PART_SIZE,show_errors);
        if(!result){
            return NULL;
        }

        
    }
    free(temp);
    return result;
    
    



}
char* ExecuteForOrdinaryCommands(struct Client* client,char * command,int PART_SIZE,bool show_errors)
{
    //execute the ordinary terminal commands 
    if(!sendMessage(client,command,PART_SIZE,show_errors)){
        return NULL;
    }

    return recieveMessage(client,PART_SIZE,show_errors);
    

}


struct Command* createCommand(struct Client* client,char* Command,int PART_SIZE){
    struct Command* full_command=(struct Command*)malloc(sizeof(struct Command*));
    full_command->client=client;
    full_command->command=Command;
    full_command->PART_SIZE=PART_SIZE;
    return full_command;


}
void* runIncommingCommands(void * _command ){
    char* result;
    struct Command* command=(struct Command*)_command;
    if((CheckForSpecialCommands(command->command))){
        result= ExecuteForSpecialCommands(command->client,command->command,command->PART_SIZE,false);
        

    }else{
        result= ExecuteForOrdinaryCommands(command->client,command->command,command->PART_SIZE,false);
        
    }

    if(!result){
        delete_client(command->client);
        
        
    }else{
        command->client->result=result;
    }
    return NULL;

}
void* keepAccepting(void * _input){
    
    struct KeepAcceptingInput* input=(struct KeepAcceptingInput*)_input;
    while(1){
        struct Client* client=acceptClient(input->server->socket_fd);
        if(!client->accept_successful){
            free(client);//the memory is dynamicly allocated so it shoul be deallocated

            continue;
        }
        if(!handShake(client,input->PART_SIZE,input->BACK_UP_PORT,input->BACK_UP_IP)){
            continue;
        }
        pthread_mutex_lock(&lock_for_clients);
        int cnt=client_cnt;
        while(clients[cnt]!=NULL){
            cnt++;
        }
        client->id=cnt;
        clients[cnt]=client;
        client_cnt++;
        client->result=NULL;
        client->deleted=false;
        pthread_cond_broadcast(&condition_var_for_clients);
        pthread_mutex_unlock(&lock_for_clients); 

        
    }
 
}

bool checkCommandIsInternal(char* command){
    char command_first[100];
    char* temp;
    if((temp=strchr(command,' '))!=NULL){
        *temp=0;
        strcpy(command_first,command);
        *temp=' ';
    }else{
        strcpy(command_first,command);
    }
    if(!strcmp(command_first,"switch")||!strcmp(command_first,"list")||!strcmp(command_first,"result")){
        return true;
    }
    return false;
}

bool executeInternalCommands(char* command,int* current_client){
    char* end_of_command=command+strlen(command);
    char* temp=command;
    while((temp=strchr(temp,' '))!=NULL){
        *temp=0;
    }

    if(!strcmp(command,"switch")){
        char* next_part=nexPart(command,end_of_command);
       // printf("%s\n",nexPart);
        if(!next_part){
            ERROR(false,"command is not correct");
            return false;
        }
        int intended_client=atoi(nexPart(command,end_of_command));
        if(clients[intended_client]){
            *current_client=intended_client;

        }else{
            ERROR(false,"no such client");
            return false;
        }
        
        return true;
    }
    if(!strcmp(command,"list")){
        for(int i=0;i<MAX_CLIENTS;i++){
            if(clients[i]!=NULL){
                if(clients[i]->deleted){
                    changeColor(RED);
                }
                printf("%s(%d)\t",clients[i]->IP,i);
                changeColor(RESET);
            }

        }

        printf("\n");
        return true;
    }
    if(!strcmp(command,"result")){
        char* next_part=nexPart(command,end_of_command);
       // printf("%s\n",nexPart);
        if(!next_part){
            ERROR(false,"command is not correct");
            return false;
        }
        int intended_client=atoi(nexPart(command,end_of_command));
        
        if(clients[intended_client]){
            if(!clients[intended_client]->result){
                
                checkAndPrintAnswer("001no results to show(the result of the previous command is not yet ready(keep in mind results will erase after each check))");
                return true;
            }
            checkAndPrintAnswer(clients[intended_client]->result);
            
            free(clients[intended_client]->result);
            clients[intended_client]->result=NULL;
            if(clients[intended_client]->deleted){
                delete_client(clients[intended_client]);
            }

        }else{
            ERROR(false,"no such client");
            return false;
        }
    }
    return false;


}
bool CheckResultNeedingCommand(char* command){
    char* temp=(char*)malloc(sizeof(char)*(strlen(command)+2));
    strcpy(temp,command);
    char* end_of_command=temp+strlen(temp);
    while(*temp==' '){
        temp++;
    }
    char* sp;
    while((sp=strchr(temp,' '))){
        *sp=0;
    }
    
    if(!strcmp(temp,"cd")){
        return true;
        free(temp);
    }
    if(!strcmp(temp,"sendall")){
        temp=nexPart(temp,end_of_command);
        return (temp && temp[0]=='c' && temp[1]=='d');
    }
    return false;

}
void delete_client(struct Client* client){
    //if result==NULL client
    pthread_mutex_lock(&lock_for_clients);
    if(client){
        if(!client->result){
            clients[client->id]=NULL;
            if(!client->deleted)//it means the cleint_cnt is decreamented once before
                client_cnt--;
            free(client);

        
        }else{
            if(!client->deleted)
                client_cnt--;
            client->deleted=true;
        }
    }   
    
    pthread_mutex_unlock(&lock_for_clients);

}
bool checkForAggregateCommands(char* command){
    char* temp=(char*)malloc(sizeof(char)*(strlen(command)+2));
    char* end_of_command=command+strlen(command);
    strcpy(temp,command);
    while(*temp==' '){
        temp++;
    }
    char* sp;
    while((sp=strchr(temp,' '))){
        *sp=0;
    }
    
    if(!strcmp(temp,"sendall")){
        return true;
        free(temp);
    }
    return false;
}
void excuteAggregateCommand(char* command,struct ThreadPool* pool,int PART_SIZE){
    char* end_of_command=command+strlen(command);
    char* temp=command;
    
    while((temp=strchr(temp,' '))!=NULL){
        *temp=0;
    }

    if(!strcmp(command,"sendall")){
        char* temp=nexPart(command,end_of_command);
        for(int i=0;i<MAX_CLIENTS;i++){
            pthread_mutex_lock(&lock_for_clients);

            if(clients[i] && !clients[i]->deleted){
                struct Command* full_command=createCommand(clients[i],temp,PART_SIZE);
                AddJob(pool,full_command);                
            }
            pthread_mutex_unlock(&lock_for_clients);


        }
        
    }
    
}




