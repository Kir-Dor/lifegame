#pragma once
#include "events/IEvent.h"

namespace event::user {

class Clear : public IEvent {
  public:
  Clear() = default;
  static constexpr std::string name() { return "User::Clear"; }
};

}