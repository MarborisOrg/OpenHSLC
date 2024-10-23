#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4
#define MAX_TASKS 100


typedef struct {
    void (*function)(void*);
    void* arg;
} Task;


typedef struct {
    Task tasks[MAX_TASKS];
    int front;
    int rear;
    int count;
} TaskQueue;

TaskQueue task_queue;
pthread_mutex_t queue_mutex;
int done = 0;


void enqueue(Task task) {
    if (task_queue.count < MAX_TASKS) {
        task_queue.tasks[task_queue.rear] = task;
        task_queue.rear = (task_queue.rear + 1) % MAX_TASKS;
        task_queue.count++;
    }
}


Task dequeue() {
    Task task = {NULL, NULL};
    if (task_queue.count > 0) {
        task = task_queue.tasks[task_queue.front];
        task_queue.front = (task_queue.front + 1) % MAX_TASKS;
        task_queue.count--;
    }
    return task;
}


void* worker_thread(void* arg) {
    while (1) {
        Task task;


        pthread_mutex_lock(&queue_mutex);
        if (task_queue.count > 0) {
            task = dequeue();
        } else if (done) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        } else {
            pthread_mutex_unlock(&queue_mutex);
            continue;
        }
        pthread_mutex_unlock(&queue_mutex);


        (*(task.function))(task.arg);
    }
    return NULL;
}


void add_task(void (*function)(void*), void* arg) {
    Task task;
    task.function = function;
    task.arg = arg;

    pthread_mutex_lock(&queue_mutex);
    enqueue(task);
    pthread_mutex_unlock(&queue_mutex);
}


void print_message(void* arg) {
    int id = *((int*)arg);
    printf("Task %d is running on thread.\n", id);

}

int main() {

    task_queue.front = 0;
    task_queue.rear = 0;
    task_queue.count = 0;

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&queue_mutex, NULL);


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker_thread, NULL);
    }


    for (int i = 0; i < 10; i++) {
        int* task_id = (int*)malloc(sizeof(int));
        *task_id = i;
        add_task(print_message, (void*)task_id);
    }


    pthread_mutex_lock(&queue_mutex);
    done = 1;
    pthread_mutex_unlock(&queue_mutex);


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&queue_mutex);
    return 0;
}
