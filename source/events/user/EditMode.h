#pragma once
#include "events/IEvent.h"

namespace event::user {

class EditMode : public IEvent {
  public:
  bool enable_flag;
  EditMode(bool enable_flag) : enable_flag(enable_flag) {};
  static constexpr std::string name() { return "User::EditMode"; }
};

}