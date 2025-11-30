#pragma once
#include "events/IEvent.h"

namespace event::user {

class SaveFile : public IEvent {
  public:
  const std::string file_name;

  SaveFile(const std::string& file_name) : file_name(file_name) {}
  static constexpr std::string name() { return "User::SaveFile"; }
};

}