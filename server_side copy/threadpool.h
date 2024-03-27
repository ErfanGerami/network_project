#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <stdio.h>
#include <pthread.h>
#include "myqueue.h"
struct ThreadPool{
    pthread_t* thread_IDs;
    int thread_num;
    struct Queue queue;
    pthread_rwlock_t lock;
    pthread_cond_t cond;
    

};
struct Input{
    void* (*func)(void*);
    struct ThreadPool* pool;
};
void* pickJob(void* inpp);
void initThreadPool(struct ThreadPool* pool,int worker_num,void*(*func)(void*));

void AddJob(struct ThreadPool* pool,void* job);

#endif