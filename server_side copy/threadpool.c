#include "threadpool.h"
void* pickJob(void* inpp){
    struct Input inp=*(struct Input*)inpp;
    while(true){
        while(true){
            //first check if a job is in reach
            pthread_rwlock_rdlock(&inp.pool->lock);
            if(inp.pool->queue.size){
                pthread_rwlock_unlock(&inp.pool->lock);
                break;
            }
            pthread_rwlock_unlock(&inp.pool->lock);

        }
        //a thread could mhave made that empty because we didnt have writer lock so we do this
        pthread_rwlock_wrlock(&inp.pool->lock);
        if(!inp.pool->queue.size){//if queue is not empty
                pthread_rwlock_unlock(&inp.pool->lock);
                continue;
        }
        void* job=dequeue(&inp.pool->queue);
        pthread_rwlock_unlock(&inp.pool->lock);
        inp.func(job);
    }
    
}
void initThreadPool(struct ThreadPool* pool,int worker_num,void*(*func)(void*)){
    pthread_rwlock_init(&pool->lock, NULL);

    pool->thread_IDs=(pthread_t*)malloc(sizeof(pthread_t)*worker_num);
    pool->thread_num=worker_num;
    initQueue(&pool->queue);
        
    struct Input* inp=(struct Input*)malloc(sizeof(struct Input));
        inp->func=func;
        inp->pool=pool;
    for(int i=0;i<worker_num;i++){

        pthread_create((pool->thread_IDs)+i,NULL,pickJob,inp);
    }

}

void AddJob(struct ThreadPool* pool,void* job){
    pthread_rwlock_wrlock(&pool->lock);
    enqueue(&pool->queue,job);
    pthread_rwlock_unlock(&pool->lock);
}

