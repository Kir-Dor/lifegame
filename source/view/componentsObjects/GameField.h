#pragma once
#include "view/IComponent.h"

#include <thread>

namespace event::model {
class FieldChanged;
}

namespace view::component {

struct GameFieldCreateInfo : public IComponentCreateInfo {};

class GameField final : public IComponent {
 public:
  GameField(GameFieldCreateInfo& create_info);
  void draw() override;
  void on_sfml_event() override {}
 private:
  mutable std::mutex mutex_;

  std::vector<sf::RectangleShape> field_;
  sf::Color color_for_dead_ = {32, 32, 32};
  sf::Color color_for_live_ = {32, 128, 32};

  void on_field_changed(const event::model::FieldChanged& event);
};

}  // namespace view::component