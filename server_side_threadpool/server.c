#define _POSIX_C_SOURCE 200809L

#include "server_client.h"
#include "helper.h"
#define  MAX_CLIENTS 60

int PORT=3007;
int PART_SIZE=1024;
int BACK_LOG=3;
int THREAD_NUM=5;
char BACK_UP_PORT[20]="UNSET";//set to 0 for unset
char BACK_UP_IP[20]="NOTSET";//set to UNSET for unset

extern struct Client* clients[MAX_CLIENTS];//max is set to be 60
extern int client_cnt;
extern pthread_mutex_t lock_for_clients;
extern pthread_cond_t condition_var_for_clients;




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
    int current_client=0;//default
    struct ThreadPool* pool=(struct ThreadPool*)malloc(sizeof(struct ThreadPool));
    //-------------------------------------------------
    //initializing--------------------------------------------
    initThreadPool(pool,THREAD_NUM,runIncommingCommands);
    initialize(argc,argv,&PORT,&BACK_LOG,&BACK_UP_PORT,&BACK_UP_IP,&PART_SIZE,&THREAD_NUM);
    pthread_mutex_init(&lock_for_clients, NULL);
    pthread_cond_init(&condition_var_for_clients,NULL);



    //---------------------------------------------------------------------------------
    struct Server* server=set_up_server(PORT,BACK_LOG);
    pthread_t accepting_thread_id;
    struct KeepAcceptingInput* input=(struct KeepAcceptingInput*)malloc(sizeof(struct KeepAcceptingInput));
    ;strcpy(input->BACK_UP_IP,BACK_UP_IP);strcpy(input->BACK_UP_PORT,BACK_UP_PORT);input->PART_SIZE=PART_SIZE;input->server=server;

    pthread_create(&accepting_thread_id,NULL,keepAccepting,input);

    printf("waiting for clients to connect\n");

    //pthread_mutex_lock(&lock_for_clients);
    pthread_cond_wait(&condition_var_for_clients,&lock_for_clients);
    pthread_mutex_unlock(&lock_for_clients);

    while(true){
        pthread_mutex_lock(&lock_for_clients);
        if(client_cnt==0){
            pthread_mutex_unlock(&lock_for_clients);
            char* command=getNotEmptyWithOutBreakLineLine(NULL);
            if(checkCommandIsInternal(command)){
                executeInternalCommands(command,&current_client);
            }else{
                ERROR(false,"command is not internal");
            }
            continue;

        }else{
            pthread_mutex_unlock(&lock_for_clients);

        }
        pthread_mutex_lock(&lock_for_clients); 
        if(!clients[current_client] || clients[current_client]->deleted ){

            if(client_cnt!=1)
                ERROR(false,"the client does not exist anymore(but you can see the last unseen result)");
            for(int i=0;i<MAX_CLIENTS;i++){
                if(clients[i]&& !clients[i]->deleted ){
                    current_client=i;
                    printf("switched to other clients\n");
                    break;
                }
            }
            pthread_mutex_unlock(&lock_for_clients);
            continue;
        }       
        pthread_mutex_unlock(&lock_for_clients);
        //getting command and answer in while loop
        char* command=getNotEmptyWithOutBreakLineLine(clients[current_client]);
        
        //running commmand-----------------------------------
        #pragma region running_command
        if(checkCommandIsInternal(command)){
            executeInternalCommands(command,&current_client);
        }else if(CheckResultNeedingCommand(command)){
            char* result=NULL;
            if((CheckForSpecialCommands(command))){
                result= ExecuteForSpecialCommands(clients[current_client],command,PART_SIZE,true);
        
            }else{
                result= ExecuteForOrdinaryCommands(clients[current_client],command,PART_SIZE,true);
        
            }
            if(!result){
                pthread_mutex_lock(&lock_for_clients);
                delete_client(clients[current_client]);
                pthread_mutex_unlock(&lock_for_clients); 
            }
        }
        else{
            if(checkForAggregateCommands(command)){
                excuteAggregateCommand(command,pool,PART_SIZE);
            }else{
                pthread_mutex_lock(&lock_for_clients); 

                if (clients[current_client]){
                
                pthread_mutex_unlock(&lock_for_clients); 
      
                    struct Command* full_command=createCommand(clients[current_client],command,PART_SIZE);
                    AddJob(pool,full_command);
                }else{
                    pthread_mutex_unlock(&lock_for_clients);

                }
            }
        }
        
    
        #pragma endregion

        



    }

       
    free(input);
    //of cource we never reach here
    pthread_mutex_destroy(&lock_for_clients);
    terminateAll(pool);
    
    

}

