#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_TASKS 500000

void after_work(uv_work_t* req, int status) {
    // This function runs after the work is completed
    free(req); // Free the request object
}

void print_message(uv_work_t* req) {
    int* id = (int*)req->data;
    printf("Task %d is running in parallel.\n", *id);
    free(id);
}

void Go(void (*task_function)(uv_work_t*), int task_id) {
    uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
    if (!req) {
        fprintf(stderr, "Memory allocation failed!\n");
        return;
    }

    int* task_data = (int*)malloc(sizeof(int));
    if (!task_data) {
        fprintf(stderr, "Memory allocation failed!\n");
        free(req);
        return;
    }

    *task_data = task_id;
    req->data = (void*)task_data;

    // Schedule the work
    uv_queue_work(uv_default_loop(), req, task_function, after_work);
}

int main() {
    // Schedule multiple tasks
    for (int i = 0; i < NUM_TASKS; i++) {
        Go(print_message, i);
    }

    // Run the event loop
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}
