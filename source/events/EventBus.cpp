#include "EventBus.h"

#include "misc/Logger.h"

namespace global {

EventBus& EventBus::get_instance() {
  static EventBus instance;
  return instance;
}

EventBus::EventBus() {
  global::Logger::get_instance().log_debug("EventBus created!");
};

}  // namespace global