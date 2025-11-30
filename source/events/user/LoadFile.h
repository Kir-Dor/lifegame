#pragma once
#include "events/IEvent.h"

namespace event::user {

class LoadFile : public IEvent {
  public:
  const std::string file_path;

  LoadFile(const std::string& file_path) : file_path(file_path) {};
  static constexpr std::string name() { return "User::LoadFile"; }
};

}