#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

void print_message(uv_work_t* req) {
    int* id = (int*)req->data;
    printf("Task %d is running in parallel.\n", *id);
    free(id);
    free(req); // Free the request object
}

void Go(void (*task_function)(uv_work_t*), int task_id) {
    uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
    int* task_data = (int*)malloc(sizeof(int));
    *task_data = task_id;
    req->data = (void*)task_data;

    uv_queue_work(uv_default_loop(), req, task_function, NULL);
}

int main() {
    for (int i = 0; i < 5; i++) {
        Go(print_message, i); // Schedule 5 tasks
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}
