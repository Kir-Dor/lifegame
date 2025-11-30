#pragma once
#include <functional>
#include <queue>
#include <thread>

namespace global {

class ThreadPool {
 public:
  static ThreadPool& get_instance();

  void add_task(std::function<void()>&& task);
  void join();
 private:
  class Worker;

  std::queue<std::function<void()>> tasks_;
  std::vector<Worker> workers_;
  std::mutex queue_mutex_;
  bool finished_;

  ThreadPool(size_t number_of_threads);
  ~ThreadPool();
  void schedule();
};

class ThreadPool::Worker {
public:
  void run();
  void set_task(std::function<void()>&& task);
  bool is_running();
  void join();
private:
  std::thread thread_;
  std::optional<std::function<void()>> task_;
  std::mutex worker_mutex_;
  std::condition_variable cv;
  std::atomic_bool running_;
  std::atomic_bool finished_;
};

}  // namespace global