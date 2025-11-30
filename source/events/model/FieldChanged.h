#pragma once
#include "events/IEvent.h"

#include <vector>

namespace event::model {

class FieldChanged final : public IEvent {
public:
  const std::vector<bool> field;

  FieldChanged(const std::vector<bool>& field) : field(field) {};
  static constexpr std::string name() { return "Model::FieldChanged"; }
};

}
