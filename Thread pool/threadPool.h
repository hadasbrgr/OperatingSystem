/*
 * hadas berger
 */

#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <sys/types.h>
#include "osqueue.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define ERROR_SIZE 21
#define FAIL "Error in system call\n"
#define FAILURE -1

enum threadPool_status {
    BEFORE = 0,
    AFTER_FREE = 1,
    RUN = 2,
    DESTROY_FUNC = 3
};

typedef struct thread_pool {
    int numOfThreads;
    int destroyOn;
    pthread_mutex_t lock;
    pthread_mutex_t destroyLock;
    pthread_t *threads;
    pthread_cond_t cond;
    OSQueue *queue;
    void (*executeTasks)(void *);
    enum threadPool_status status;
} ThreadPool;

typedef struct task {
    void (*computeFunc) (void *);
    void* param;
} TaskData;

/**
 * check if the aloc work
 * @param num
 */
void checkError(int num);
/**
 * The function that each thread runs
 * @param arg
 */
void executeTasks(void* arg);
/**
 * A static wrapper that calls executeTasks()
 * @param arg
 * @return
 */
void* execute(void* arg);
/**
 * create the threadPool in size of the numOfThreads
 * @param numOfThreads
 * @return pointer to the threadPool we create
 */
ThreadPool* tpCreate(int numOfThreads);
/**
 * destroy the ThreadPool
 * @param threadPool - to destroy
 * @param shouldWaitForTasks - choose what to do:
 * if == 0: we wait just to the thread that run and than free all
 * if ==1: we wait until all the task in the queue will finish.
 * but, we block enter of new tasks
 */
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);
/**
 * add new task to queue.
 * @param threadPool
 * @param computeFunc
 * @param param
 * @return 0 if success. -1 if the function "destroy" call in the same time.
 */
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);
/**
 * free all the allocation in the program
 * @param pool
 */
void freeTheThread(ThreadPool* pool);
#endif