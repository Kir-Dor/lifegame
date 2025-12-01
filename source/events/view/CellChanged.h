#pragma once
#include "events/IEvent.h"

namespace event::view {

class CellChanged : public IEvent {
  public:
  const int x, y;
  bool value;
  CellChanged(int x, int y, bool value) : x(x), y(y), value(value) {}
  static constexpr std::string name() { return "View::CellChanged"; }
};

}