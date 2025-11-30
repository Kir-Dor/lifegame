#include "ThreadPool.h"

#include <mutex>

#include "Logger.h"
#include "events/EventBus.h"

namespace global {

ThreadPool& ThreadPool::get_instance() {
  static ThreadPool threadPool(std::thread::hardware_concurrency() - 1);
  return threadPool;
}

void ThreadPool::add_task(std::function<void()>&& task) {
  std::unique_lock lock(queue_mutex_);
  tasks_.push(std::forward<std::function<void()>>(task));
  lock.unlock();

  global::Logger::get_instance().log_trace("ThreadPool: Task added!");

  schedule();
}

void ThreadPool::join() {
  std::unique_lock lock(queue_mutex_);
  if (finished_) {
    return;
  }
  finished_ = true;
  global::Logger::get_instance().log_trace("ThreadPool: flag finished on!");
  while (!tasks_.empty()) {
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    lock.lock();
  }
  global::Logger::get_instance().log_trace("ThreadPool: Task queue cleared!");
  lock.unlock();
  for (auto& worker : workers_) {
    worker.join();
  }
  global::Logger::get_instance().log_debug("ThreadPool: All workers joined!");
}

ThreadPool::ThreadPool(size_t number_of_threads) : workers_(number_of_threads) {
  finished_ = false;
  for (auto& worker : workers_) {
    worker.run();
  }
  global::Logger::get_instance().log_debug("ThreadPool created!");
}

ThreadPool::~ThreadPool() {
  join();
  global::Logger::get_instance().log_debug("ThreadPool destroyed!");
}

void ThreadPool::schedule() {
  std::unique_lock lock(queue_mutex_);
  if (tasks_.empty()) {
    return;
  }
  for (auto& worker : workers_) {
    if (!worker.is_running()) {
      worker.set_task(std::move(tasks_.front()));
      tasks_.pop();
      global::Logger::get_instance().log_trace("ThreadPool: task from queue given to worker!");
      break;
    }
  }
}

}  // namespace global

namespace global {

void ThreadPool::Worker::run() {
  auto run_handler = [this] {
    global::Logger::get_instance().log_trace("Worker: run!");
    while (true) {
      std::unique_lock lock(worker_mutex_);

      cv.wait(lock, [this] { return task_.has_value() || finished_; });

      if (finished_) {
        return;
      }

      running_ = true;
      task_.value()();
      global::Logger::get_instance().log_trace("ThreadPool: task completed!");
      task_.reset();
      running_ = false;
    }
  };
  thread_ = std::thread(run_handler);
}

void ThreadPool::Worker::set_task(std::function<void()>&& task) {
  if (!running_) {
    std::unique_lock lock(worker_mutex_);
    task_ = std::forward<std::function<void()>>(task);
    lock.unlock();
    cv.notify_all();
    global::Logger::get_instance().log_trace("Worker: Task given!");
  }
}

bool ThreadPool::Worker::is_running() {
  return running_;
}

void ThreadPool::Worker::join() {
  finished_ = true;
  global::Logger::get_instance().log_trace("Worker: flag finished on!");
  while (running_) {}
  cv.notify_all();
  global::Logger::get_instance().log_trace("Worker: flag running off!");
  thread_.join();
  global::Logger::get_instance().log_trace("ThreadPool: Worker joined!");
}

}