#pragma once
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

#include "IEvent.h"
#include "ISubscriber.h"
#include "misc/LambdaCreator.h"
#include "misc/Logger.h"
#include "misc/ThreadPool.h"

namespace global {

class EventBus {
 public:
  static EventBus& get_instance();

  template <event::EventConcept Event, auto MethodPtr,
            event::SubscribeConcept Sub>
  void subscribe(Sub& subscriber);

  template <event::EventConcept Event, auto MethodPtr,
            event::SubscribeConcept Sub>
  void unsubscribe(Sub& subscriber);

  template <event::EventConcept T>
  void invoke(T&& event);

 private:
  std::unordered_multimap<std::string,
                          std::tuple<event::ISubscriber&,
                                     std::function<void(const event::IEvent&)>>>
      action_map_;
  std::mutex mutex_;
  size_t action_map_size = 0;

  EventBus();
};

}  // namespace global

namespace global {
template <event::EventConcept Event, auto MethodPtr,
          event::SubscribeConcept Sub>
void EventBus::subscribe(Sub& subscriber) {
  std::unique_lock lock(mutex_);
  global::Logger::get_instance().log_debug(
      R"(Action with type id "{}" subscribed on event "{}")",
      typeid(MethodPtr).name(), Event::name());

  std::function<void(const Event&)> action =
      misc::LambdaCreator::create<MethodPtr>(subscriber);

  std::function<void(const event::IEvent&)> wrapper =
      [local_action = std::move(action)](const event::IEvent& event) {
        local_action(static_cast<const Event&>(event));
      };

  action_map_.insert(std::make_pair(
      Event::name(), std::tuple<event::ISubscriber&,
                                std::function<void(const event::IEvent&)>>(
                         subscriber, std::move(wrapper))));
  action_map_size = action_map_.size();
}

template <event::EventConcept Event, auto MethodPtr,
          event::SubscribeConcept Sub>
void EventBus::unsubscribe(Sub& subscriber) {
  std::unique_lock lock(mutex_);

  std::function<void(const Event&)> action =
      misc::LambdaCreator::create<MethodPtr>(subscriber);

  std::function<void(const event::IEvent&)> wrapper =
      [local_action = std::move(action)](const event::IEvent& event) {
        local_action(static_cast<const Event&>(event));
      };

  auto [begin, end] = action_map_.equal_range(Event::name());
  for (auto it = begin; it != end; ++it) {
    if (&std::get<0>(it->second) == &static_cast<event::ISubscriber&>(subscriber) &&
         typeid(std::get<1>(it->second)) == typeid(wrapper)) {
      action_map_.erase(it);
      global::Logger::get_instance().log_debug(
          R"(Action with type id "{}" unsubscribed from event "{}")",
          typeid(MethodPtr).name(), Event::name());
      break;
    }
  }
  action_map_size = action_map_.size();
}

template <event::EventConcept T>
void EventBus::invoke(T&& event) {
  std::unique_lock lock(mutex_);
  global::Logger::get_instance().log_debug(R"(Event "{}" invoked)", T::name());
  auto [begin, end] = action_map_.equal_range(T::name());
  for (auto it = begin; it != end; ++it) {
    auto task = [action = std::get<1>(it->second), event] {
      global::Logger::get_instance().log_trace("EventBus task invoked");
      action(event);
    };
    global::ThreadPool::get_instance().add_task(std::move(task));
  }
}

}  // namespace global
