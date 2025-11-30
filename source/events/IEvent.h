#pragma once
#include <string>

namespace event {

class IEvent {
public:
  virtual ~IEvent() = default;
};

template <typename T>
concept EventConcept = requires {
  std::is_base_of_v<IEvent, T>;
  { T::name() } -> std::same_as<std::string>;
};

}