#pragma once
#include <thread>

#include "events/ISubscriber.h"
#include "view/IComponent.h"

namespace event::user {
class EditMode;
}
namespace event::model {
class FieldChanged;
class CellChanged;
}

namespace view::component {

struct GameFieldCreateInfo : public IComponentCreateInfo {};

class GameField final : public IComponent, public event::ISubscriber {
 public:
  GameField(GameFieldCreateInfo& create_info);
  ~GameField();

  void draw() override;
  void on_sfml_event() override;
 private:
  mutable std::mutex mutex_;
  std::atomic_bool edit_mode;

  std::vector<sf::RectangleShape> field_;
  sf::Color color_for_dead_ = {32, 32, 32};
  sf::Color color_for_live_ = {32, 128, 32};

  void on_field_changed(const event::model::FieldChanged& event);
  void on_cell_changed(const event::model::CellChanged& event);
  void on_edit_mode(const event::user::EditMode& event);
};

}  // namespace view::component