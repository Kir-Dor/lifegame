#pragma once
#include <functional>
#include <events/IEvent.h>

namespace misc {

class LambdaCreator {
  public:
  template <auto MethodPtr>
  static constexpr auto create(auto& object) {
    auto lambda = [&object](const auto& event) {
      (object.*MethodPtr)(event);
    };
    return lambda;
  }
};

}