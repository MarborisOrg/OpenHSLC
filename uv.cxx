#include <uv.h>
#include <iostream>
#include <memory>
#include <vector>

#define NUM_TASKS 500000

void after_work(uv_work_t *req, int status) {
  // This function runs after the work is completed
  auto task_data = static_cast<std::unique_ptr<int> *>(req->data);
  delete task_data; // Free the unique_ptr
  delete req;       // Free the request object
}

void print_message(uv_work_t *req) {
  auto task_data = static_cast<std::unique_ptr<int> *>(req->data);
  std::cout << "Task " << **task_data << " is running in parallel." << std::endl;
}

void Go(void (*task_function)(uv_work_t *), int task_id) {
  uv_work_t *req = new uv_work_t;
  auto task_data = new std::unique_ptr<int>(std::make_unique<int>(task_id));
  req->data = static_cast<void *>(task_data);

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
