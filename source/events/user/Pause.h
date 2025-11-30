#pragma once
#include "events/IEvent.h"

namespace event::user {

class Pause : public IEvent {
 public:
  Pause() = default;
  static constexpr std::string name() { return "User::Pause"; }
};

}  // namespace event::user