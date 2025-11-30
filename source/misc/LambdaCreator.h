#pragma once

namespace misc {

class LambdaCreator {
  public:
  template <auto MethodPtr>
  static constexpr auto create(auto& object) {
    return [&object](const auto& event) {
      (object.*MethodPtr)(event);
    };
  }
};

}