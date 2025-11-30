#pragma once
#include "events/IEvent.h"

namespace event::user {

class Tick : public IEvent {
  public:
  size_t iteration_count;

  Tick(size_t iteration_count = 1) : iteration_count(iteration_count) {}
  static constexpr std::string name() { return "User::Tick"; }
};

}