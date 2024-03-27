#include "threadpool.h"

void initThreadPool(struct ThreadPool* pool,int worker_num,void*(*func)(void*)){
    pthread_rwlock_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);   
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
    pthread_rwlock_wrlock(&pool->lock);
    enqueue(&pool->queue,job);
    

    pthread_rwlock_unlock(&pool->lock);
}

void* pickJob(void* inpp) {
    struct Input* inp = (struct Input*)inpp;
    while (true) {
        pthread_rwlock_rdlock(&inp->pool->lock);

        while (!inp->pool->queue.size) {
            // Wait for the queue to become non-empty
            pthread_cond_wait(&inp->pool->cond, &inp->pool->lock);
        }
        pthread_rwlock_unlock(&inp->pool->lock);
        pthread_rwlock_wrlock(&inp->pool->lock);
        void* job = dequeue(&inp->pool->queue);
        pthread_rwlock_unlock(&inp->pool->lock);
        inp->func(job);
    }
}