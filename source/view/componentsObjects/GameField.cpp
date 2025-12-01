#include "GameField.h"

#include <SFML/Graphics.hpp>
#include <mutex>
#include <vector>

#include "Console.h"
#include "events/EventBus.h"
#include "events/model/CellChanged.h"
#include "events/model/FieldChanged.h"
#include "events/user/EditMode.h"
#include "events/view/CellChanged.h"
#include "misc/Config.h"

namespace view::component {

GameField::GameField(GameFieldCreateInfo& create_info)
    : IComponent(create_info) {
  component_bounds_.setFillColor(sf::Color::Black);

  field_.resize(Config::FIELD_WIDTH * Config::FIELD_HEIGHT,
                sf::RectangleShape());
  for (auto i = 0; i < Config::FIELD_WIDTH; ++i) {
    for (auto j = 0; j < Config::FIELD_HEIGHT; ++j) {
      field_[i * Config::FIELD_HEIGHT + j].setFillColor(color_for_dead_);
      field_[i * Config::FIELD_HEIGHT + j].setSize({16, 16});
      field_[i * Config::FIELD_HEIGHT + j].setPosition({position_.x + 6 + 18 * i, position_.y + 4 + 18 * j});
    }
  }

  auto& event_bus = global::EventBus::get_instance();

  event_bus.subscribe<event::model::FieldChanged, &GameField::on_field_changed>(*this);
  event_bus.subscribe<event::model::CellChanged, &GameField::on_cell_changed>(*this);
  event_bus.subscribe<event::user::EditMode, &GameField::on_edit_mode>(*this);
}

GameField::~GameField() {
  auto& event_bus = global::EventBus::get_instance();

  event_bus.unsubscribe<event::model::FieldChanged, &GameField::on_field_changed>(*this);
  event_bus.unsubscribe<event::model::CellChanged, &GameField::on_cell_changed>(*this);
  event_bus.unsubscribe<event::user::EditMode, &GameField::on_edit_mode>(*this);
}

void GameField::draw() {
  std::unique_lock lock(mutex_);
  window_.draw(component_bounds_);
  for (auto& cell : field_) {
    window_.draw(cell);
  }
}

void GameField::on_sfml_event() {
  if (!edit_mode) {
    return;
  }
  if (auto mouse_click = current_event_->getIf<sf::Event::MouseButtonPressed>()) {
    if (mouse_click->button == sf::Mouse::Button::Left) {
      auto [cell_x, cell_y] = mouse_click->position;
      cell_x = (cell_x - position_.x - 6) / 18;
      cell_y = (cell_y - position_.y - 4) / 18;
      auto [temp_x, temp_y] = field_[cell_x * Config::FIELD_HEIGHT + cell_y].getPosition();
      auto [mouse_x, mouse_y] = mouse_click->position;
      if (mouse_x > temp_x + 16 || mouse_y > temp_y + 16) {
        return;
      }
      bool cell_value = field_[cell_x * Config::FIELD_HEIGHT + cell_y].getFillColor() == color_for_dead_;
      global::EventBus::get_instance().invoke(event::view::CellChanged(cell_x, cell_y, cell_value));
    }
  }
}

void GameField::on_field_changed(
    const event::model::FieldChanged& event) {
  std::unique_lock lock(mutex_);
  for (auto i = 0; i < event.field.size(); ++i) {
    if (event.field[i]) {
      field_[i].setFillColor(color_for_live_);
    }
    else {
      field_[i].setFillColor(color_for_dead_);
    }
  }
}

void GameField::on_cell_changed(const event::model::CellChanged& event) {
  std::unique_lock lock(mutex_);
  if (event.value) {
    field_[event.x * Config::FIELD_HEIGHT + event.y].setFillColor(color_for_live_);
  }
  else {
    field_[event.x * Config::FIELD_HEIGHT + event.y].setFillColor(color_for_dead_);
  }
}

void GameField::on_edit_mode(const event::user::EditMode& event) {
  std::unique_lock lock(mutex_);
  edit_mode = event.enable_flag;
}

}  // namespace view::component