#pragma once
#include "events/IEvent.h"

namespace event::user {

class Stop : public IEvent {
 public:
  Stop() = default;
  static constexpr std::string name() { return "User::Stop"; }
};

}  // namespace event::user