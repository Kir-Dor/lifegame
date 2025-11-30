#pragma once
#include <SFML/Graphics.hpp>

namespace view {
namespace component {
class IComponent;
}

class GameView {
 public:
  void display();

 private:
  sf::RenderWindow window_;
  std::optional<sf::Event> current_window_event_;

  std::chrono::microseconds frame_duration_ = std::chrono::microseconds::zero();
  std::chrono::microseconds fps_tick_duration = std::chrono::microseconds::zero();
  float fps_sum_ = 0;
  uint32_t sum_counter_ = 0;

  float fps_ = 0;

  std::vector<std::unique_ptr<component::IComponent>> components_;

  void initialize();
  void fps_update();
};

}  // namespace view