#include "myqueue.h"
void initQueue(struct Queue* q){
    //initiates the queue and allocate a pre defined amount of  memory
    q->arr=(void**)malloc(sizeof(void*)*INIT_CAPACITY);
    q->capacity=INIT_CAPACITY;
    q->size=0;
    q->front=0;

    

}
void enqueue(struct Queue* q,void * data){
    //adds to the queue
    if(q->capacity==q->size){
        void** temp=(void**)malloc(sizeof(void*)*q->capacity*2);
        for(int i=1;i<q->size+1;i++){
            temp[i-1]=q->arr[(q->front+i)%q->capacity];
        }
        q->capacity*=2;
        free(q->arr);
        q->arr=temp; 
        q->front=q->capacity-1 ;
    }
    
    q->arr[q->front]=data;
    q->front=(q->front-1+q->capacity)%q->capacity;
    q->size++;
    

}
void* dequeue(struct Queue* q){
    //removes the first element in the queue
    if(q->size){
        q->front=(q->front+1)%q->capacity;
        q->size--;
        return q->arr[q->front];
    }
    return NULL;
}
void printQueue(struct Queue * q){
    printf("queue:");
    for(int i=1;i<q->size+1;i++){
        printf("%p-",q->arr[(q->front+i+q->capacity)%q->capacity]);//assumed int for debug puposes
    }
    printf("\n");
   
}