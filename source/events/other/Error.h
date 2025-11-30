#pragma once
#include "events/IEvent.h"

namespace event {

class Error : public IEvent {
  public:
    const std::string error_message;

    Error(const std::string& error_message) : error_message(error_message) {}
    static constexpr std::string name() { return "Error"; };
};

}