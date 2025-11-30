#pragma once
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

#include "IEvent.h"
#include "misc/Logger.h"
#include "misc/ThreadPool.h"

namespace global {

class EventBus {
 public:
  static EventBus& get_instance();

  template <event::EventConcept T>
  void subscribe(std::function<void(const T&)>&& action);

  template <event::EventConcept T>
  void unsubscribe(std::function<void(const T&)>&& action);

  template <event::EventConcept T>
  void invoke(T&& event);

 private:
  std::unordered_multimap<std::string, std::function<void(const event::IEvent&)>> action_map;
  std::mutex mutex_;
  size_t action_map_size = 0;

  EventBus();
};

}  // namespace global

namespace global {
template <event::EventConcept T>
void EventBus::subscribe(std::function<void(const T&)>&& action) {
  std::unique_lock lock(mutex_);
  global::Logger::get_instance().log_debug(R"(Action with type id "{}" subscribed on event "{}")", typeid(action).name(), T::name());
  std::function<void(const event::IEvent&)> wrapper = [local_action = std::forward<std::function<void(const T&)>>(action)](const event::IEvent& event) {
    local_action(static_cast<const T&>(event));
  };
  action_map.insert(std::make_pair(T::name(), std::move(wrapper)));
  action_map_size = action_map.size();
}

template <event::EventConcept T>
void EventBus::unsubscribe(std::function<void(const T&)>&& action) {
  std::unique_lock lock(mutex_);
  std::function<void(const event::IEvent&)> wrapper = [local_action = std::forward<std::function<void(const T&)>>(action)](const event::IEvent& event) {
    global::Logger::get_instance().log_trace("EventBus wrapper invoked");
    local_action(static_cast<const T&>(event));
  };
  auto [begin, end] = action_map.equal_range(T::name());
  for (auto it = begin; it != end; ++it) {
    if (typeid(it->second) == typeid(wrapper)) {
      action_map.erase(it);
      global::Logger::get_instance().log_debug(R"(Action with type id "{}" unsubscribed from event "{}")", typeid(action).name(), T::name());
      break;
    }
  }
  action_map_size = action_map.size();
}

template <event::EventConcept T>
void EventBus::invoke(T&& event) {
  std::unique_lock lock(mutex_);
  global::Logger::get_instance().log_debug(R"(Event "{}" invoked)", T::name());
  auto [begin, end] = action_map.equal_range(T::name());
  for (auto it = begin; it != end; ++it) {
    auto task = [action = it->second, event] {
      global::Logger::get_instance().log_trace("EventBus task invoked");
      action(event);
    };
    global::ThreadPool::get_instance().add_task(std::move(task));
  }
}

}
