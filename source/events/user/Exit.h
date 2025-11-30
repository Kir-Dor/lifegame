#pragma once
#include "events/IEvent.h"

namespace event::user {

class Exit : public IEvent {
public:
  Exit() = default;
  static constexpr std::string name() { return "User::Exit"; }
};

}