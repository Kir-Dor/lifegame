#pragma once
#include "events/IEvent.h"

namespace event::user {

class Start : public IEvent {
 public:
  Start() = default;
  static constexpr std::string name() { return "User::Start"; }
};

}  // namespace event::user