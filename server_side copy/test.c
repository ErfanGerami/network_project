#include "myqueue.h"
#include "threadpool.h"
#include <string.h>
void* print(void* message){
    

    printf("%s\n",(char*)message);
    return NULL;

}
int  main(){
    struct ThreadPool* pool=(struct ThreadPool*)malloc(sizeof(struct ThreadPool));
    char* message="salam";
    initThreadPool(pool,2,print);
    for(int i=0;i<4;i++){
        
       char c[10];
        strcpy(c,message);
        AddJob(pool,c);
    }

    
    while(1){}
    

}