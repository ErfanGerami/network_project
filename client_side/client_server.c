#include "client_server.h"
#include <pthread.h>
bool sendMessage(struct Client* client,char* message,int PART_SIZE){  
     //sends the lenght of the message in 20 chars so other side knows how much should it read 
    //then sends the actual data  and if everything goes as expected it returns true otherwise false  
    char size_of_result[21]={0};
    sprintf(size_of_result,"%d",strlen(message));
    //the size of number should be exactly 20
    //because we have two sends in continous we should specifiy their size
    if(send(client->socket_fd,size_of_result,20,0)<=0){
        ERROR(false,"error while sending size");
        return false;
    }
    int size_of_message=strlen(message);
    int sent=0;
    
    //it sends data in parts until all the data is send
    while(sent<size_of_message){
        int data_to_send=min(PART_SIZE,size_of_message-sent);
        if(send(client->socket_fd,message+sent,data_to_send,0)<data_to_send){
            ERROR(false,NULL);
            return false;
        }
        sent+=data_to_send;
    }
    return true;

}
char* recieveMessage(struct Client* client,int PART_SIZE){
    //firsts recieve the size of data in 20 chars and then recieves the actual data and return it 
    //if anything goes wrong it will return NUL


    char* answer_size_string[21]={0};
    if(recv(client->socket_fd,answer_size_string,20,0)<=0){//I read exacly 20 
            ERROR(false,NULL);
            return NULL;
    }
    int recieving_data_size=atoi(answer_size_string);
    char* answer=(char*)calloc(recieving_data_size+3,sizeof(char));
    int recieved=0;
    
    while(recieved<recieving_data_size){
        int data_to_recieve=min(PART_SIZE,recieving_data_size-recieved);
        if(recv(client->socket_fd,answer+recieved,data_to_recieve,0)<=0){
            ERROR(false,NULL);
            return NULL;
        }
        recieved+=data_to_recieve;
    }
    return answer;
}
struct Client*  set_up_client(int PORT,char* IP){
    struct Client* client=(struct Client*)malloc(sizeof(struct Client));
    client->socket_fd=socket(AF_INET,SOCK_STREAM,0);
    
    if(client->socket_fd<-1){
        ERROR(false,"error when constructing socket");
        return NULL;
    }
    client->addr.sin_port=htons(PORT);
    client->addr.sin_family=AF_INET;
    inet_pton(AF_INET,IP,&(client->addr.sin_addr.s_addr));

    if(connect(client->socket_fd,&(client->addr),sizeof(struct sockaddr_in))==-1){
        ERROR(false,"error while connecting");
        return NULL;
    }
    printf("connected to server successfully\n");
    return client;


}
bool handShake(struct Client* client,int* PART_SIZE,int* BACK_UP_PORT, char* BACK_UP_IP){
    //recieving rules-----------------------------
    char buffer[200];
    if(recv(client->socket_fd,buffer,sizeof(buffer),0)<=0){
        ERROR(false,"error while handshaking");
        return false;
    }
    printf("%s\n",buffer);
    sscanf(buffer,"%d %d %s",PART_SIZE,BACK_UP_PORT,BACK_UP_IP);

    printf("handshake was successful\n");
    printf("PART_SIZE:%d BACK_UP_PORT:%d BACK_UP_IP:%s\n",*PART_SIZE,*BACK_UP_PORT,BACK_UP_IP);
    //sending path-----------------------------------------
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if(!sendMessage(client,cwd,PART_SIZE)){
        ERROR(false,"error while handshaking");
        return false;
        pthread_create()
        
    }



    return true;
}
char* runCommand(char* command){
    //the one char in the beginning of the result is for exit code
    int capacity=200;
    const int segment_size=1035; 
    int fulled_size=0;
    char* result=(char*)calloc((capacity+4),sizeof(char));
    //preserved for exit code 
    result[0]='0';
    result[1]='0';
    result[2]='0';
    

    FILE *fp;
    strcat(command," 2>&1");
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        strcat(result,"1Failed to run command");
    }else{
        

        char* temp=(char*)malloc(sizeof(char)*segment_size);
        while (fgets(temp, segment_size*sizeof(char), fp) != NULL) {
            strcat(result,temp);
            fulled_size+=segment_size;
            if(fulled_size>capacity/2){
                capacity*=2;
                result=(char*)realloc(result,capacity*sizeof(char));
            }
        
        }
    }
        int exit_code=WEXITSTATUS(pclose(fp));
        result[0]=exit_code/100+'0';
        result[1]=(exit_code/10)%10+'0';
        result[2]=(exit_code)%10+'0';   
    
    
    
    return result;
}
bool getAndRunCommand(struct Client* client,int PART_SIZE){
    char* command=recieveMessage(client,PART_SIZE);
    printf("command:%s\n",command);
    char* result;
    if(!(result=CheckForSpecialCommands(client,command))){
        result=runCommand(command);
    }
    printf("result:%s\n",result);
    
    bool sent= sendMessage(client,result,PART_SIZE);
    free(command);
    free(result);
    return sent;

    

}
void fillResultWithExitCode(char* result,int exit_code){
    result[0]=exit_code/100+'0';
    result[1]=(exit_code/10)%10+'0';
    result[2]=(exit_code)%10+'0'; 
}

char* CheckForSpecialCommands(struct Client* client,char * command){
    char* result=NULL;
    char* temp=(char*)malloc(sizeof(char)*(strlen(command)+2));
    strcpy(temp,command);
    while(*temp==' '){
        temp++;
    }
    char* sp;
    //replacing parts with \0
    while((sp=strchr(temp,' '))){
        *sp=0;

        
    }
    
    if(!strcmp(temp,"cd")){
        result =runCommand(command);
        printf("%s\n",result);
        if(chdir(temp+strlen(temp)+1)){
            result[2]='1';//just to have non zero exit code
        }else{
            getcwd(client->path,NAME_MAX);
            result[3]=0;
            result=(char*)realloc(result,sizeof(char)*(strlen(client->path)+5));
            strcat(result,client->path);

        }
       
    }
    if(!strcmp(temp,"get_file")){
        FILE* file=fopen(temp+strlen(temp)+1,"r");
        result=(char*)malloc(sizeof(char)*101);
        if(!file){

            fillResultWithExitCode(result,1);
            result[3]=0;
            strcat(result,"couldnt open file");
        }
        else{
            fillResultWithExitCode(result,0);
            result[3]=0;
            char temp[100]={0};
            int curr_size=101;
            while(fgets(temp,sizeof(temp),file)){
                curr_size+=101;
                result=(char*)realloc(result,curr_size*sizeof(char*));
                strcat(result,temp);
            }
            fclose(file);   
        }
       
    }
    if(!strcmp(temp,"send_file")){
        result=(char*)malloc(sizeof(char)*101);
        char* next_part=temp+strlen(temp)+1;
        *strchr(next_part,' ')=0;
        FILE* file=fopen(next_part,"w");
        if(!file){
            fillResultWithExitCode(result,1);
            result[3]=0;
            strcat(result,"opening file failed");
        }
        else{
            fillResultWithExitCode(result,0);
            result[3]=0;
            strcat(result,"copied");
            next_part+=strlen(next_part)+1;
            fprintf(file,next_part);
            fclose(file);   
        }

    }


    free(temp);
    return result;//if any of above conditions are not satisfied result will remian NULL
    
    



}
