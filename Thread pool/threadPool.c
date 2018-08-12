/*
 * hadas berger
 */

#include "threadPool.h"

void checkError(int num) {
    int out = open("outFile",  O_WRONLY | O_TRUNC |
                           O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if (num == FAILURE ){
        dup2(out,1);
        write(2, FAIL, ERROR_SIZE);
        dup2(1,out);
        exit(FAILURE);
    }
}

void executeTasks(void* arg) {

    ThreadPool *pool = (ThreadPool*) arg;

    while(!(pool->status==AFTER_FREE)){
        //if we call to destroy function and the queue is empty
        if(pool->status==DESTROY_FUNC && osIsQueueEmpty(pool->queue)) {
            break;
        //if the queue is empty and we run regular/we need to finish the run task - wait
        } else if(osIsQueueEmpty(pool->queue) &&
                 (pool->status==RUN || pool->status==BEFORE)) {
            pthread_mutex_lock(&pool->lock);
            pthread_cond_wait(&(pool->cond), &pool->lock);
        }else {
            pthread_mutex_lock(&pool->lock);
        }

        if(!(pool->status==AFTER_FREE)) {
            //when the queue not empty
            if (!osIsQueueEmpty(pool->queue)) {
                //play task from the queue
                TaskData *task = osDequeue(pool->queue);
                pthread_mutex_unlock(&pool->lock);
                task->computeFunc(task->param);
                free(task);
            } else {
                pthread_mutex_unlock(&pool->lock);
            }
            //if we in destroy and shouldWaitForTasks==0
            if (pool->status == BEFORE) {
                break;
            }

        }else{
            pthread_mutex_unlock(&pool->lock);
        }
    }
}

void* execute(void* arg) {
    ThreadPool *pool = (ThreadPool*) arg;
    pool->executeTasks(arg);
}

ThreadPool* tpCreate(int numOfThreads){

    ThreadPool *tp;
    tp = (ThreadPool*)malloc(sizeof(ThreadPool));
    if(tp==NULL) {
        checkError(FAILURE);
    }
    //initialize all the fields
    tp->numOfThreads=numOfThreads;
    tp->destroyOn=0;
    tp->status=RUN;
    tp->executeTasks=executeTasks;
    tp->queue=osCreateQueue();
    int check1=pthread_cond_init(&tp->cond,NULL);
    checkError(check1);
    int check=pthread_mutex_init(&tp->lock,NULL);
    checkError(check);
    int check2=pthread_mutex_init(&tp->destroyLock,NULL);
    checkError(check2);
    tp->threads = (pthread_t*)malloc(sizeof(pthread_t)*numOfThreads);
    if(tp->threads == NULL) {
        checkError(FAILURE);
    }
    for (int i = 0; i < numOfThreads; i++) {
        pthread_create(&(tp->threads[i]), NULL,execute,tp);
    }
    return tp;
}

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param) {

    if (threadPool->destroyOn) {    //if we called to destroy function- block new tasks
        return FAILURE;
    } else {
        TaskData *data = (TaskData *) malloc(sizeof(TaskData));
        if (data == NULL) {
            checkError(FAILURE);
        }
        data->computeFunc = computeFunc;
        data->param = param;

        pthread_mutex_lock(&threadPool->lock);
        osEnqueue(threadPool->queue, data);

        if (pthread_cond_signal(&threadPool->cond) != 0) {
            checkError(FAILURE);
        }
        pthread_mutex_unlock(&threadPool->lock);

        return 0;
    }
}

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks){

    pthread_mutex_lock(&threadPool->destroyLock);
    //if we try to call to destroy when we in the middle of one
    if(threadPool->destroyOn) {
        return;
    }
    pthread_mutex_unlock(&threadPool->destroyLock);

    pthread_mutex_lock(&threadPool->lock);
    if((pthread_cond_broadcast(&threadPool->cond)!=0)||
            (pthread_mutex_unlock(&threadPool->lock)!=0)){
        checkError(FAILURE);
    }
    //wait just to running threads
    if(shouldWaitForTasks == 0) {
        threadPool->status = BEFORE;
    //wait to all the task in queue over
    }else{
        threadPool->status = DESTROY_FUNC;
    }
    freeTheThread(threadPool);

}

void freeTheThread(ThreadPool* pool){
    pool->destroyOn = 1;

    for (int i = 0; i < pool->numOfThreads; ++i) {
        pthread_join(pool->threads[i], NULL);
    }

    while (!osIsQueueEmpty(pool->queue)) {
        TaskData* task = osDequeue(pool->queue);
        free(task);
    }

    pool->status = AFTER_FREE;
    free(pool->threads);
    pthread_mutex_destroy(&pool->lock);
    pthread_mutex_destroy(&pool->destroyLock);
    pthread_cond_destroy(&pool->cond);
    osDestroyQueue(pool->queue);
    free(pool);
}