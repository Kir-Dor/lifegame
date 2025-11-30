#pragma once
#include <cstdint>

class Config {
public:
  static constexpr uint16_t FIELD_WIDTH = 106;
  static constexpr uint16_t FIELD_HEIGHT = 58;
  static constexpr float GAME_TICK_RATE = 20;
  static constexpr float FPS_TICK_RATE = 5;
};