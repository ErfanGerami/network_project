#ifndef MYQUEUE_H
#define MYQUEUE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define INIT_CAPACITY 2
struct Queue{
    void** arr;
    int size;
    int capacity;
    int front;
};

void initQueue(struct Queue* q);
void enqueue(struct Queue* q,void * data);
void* dequeue(struct Queue* q);
void printQueue(struct Queue * q);
#endif