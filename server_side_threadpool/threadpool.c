#include "threadpool.h"

void initThreadPool(struct ThreadPool* pool,int worker_num,void*(*func)(void*)){
      
    pthread_mutex_init(&pool->queue_lock, NULL);
    pthread_cond_init(&pool->queue_cond, NULL);
    pool->thread_IDs=(pthread_t*)malloc(sizeof(pthread_t)*worker_num);
    pool->thread_num=worker_num;
    initQueue(&pool->queue);
        
    for(int i=0;i<worker_num;i++){
    struct Input* inp=(struct Input*)malloc(sizeof(struct Input));
        inp->func=func;
        inp->pool=pool;

        pthread_create((pool->thread_IDs)+i,NULL,pickJob,inp);
    }

}

void AddJob(struct ThreadPool* pool,void* job){
    pthread_mutex_lock(&pool->queue_lock); 

    enqueue(&pool->queue, job);
    pthread_mutex_unlock(&pool->queue_lock);
    pthread_cond_broadcast(&pool->queue_cond); 
}

void* pickJob(void* inpp) {
    struct Input* inp = (struct Input*)inpp;
    while (true) {
        pthread_mutex_lock(&inp->pool->queue_lock); 

        while (!(&inp->pool->queue)->size) {
    //printQueue(&inp->pool->queue);
            // waiting for the queue to become non empty
            pthread_cond_wait(&inp->pool->queue_cond, &inp->pool->queue_lock);
        }
        pthread_mutex_unlock(&inp->pool->queue_lock); 

        void* job = dequeue(&inp->pool->queue);

        inp->func(job);
    }
}
void terminateAll(struct ThreadPool* pool){
    for (int i=0;i<pool->thread_num;i++){
        pthread_cancel(pool->thread_IDs[i]);
        pthread_join(pool->thread_IDs[i], NULL);
    }
}