#pragma once
#include "events/IEvent.h"

namespace event::view {

class Resume : public IEvent {
 public:
  Resume() = default;
  static constexpr std::string name() { return "View::Resume"; }
};

}  // namespace event
