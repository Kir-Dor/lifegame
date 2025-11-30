#include "GameField.h"

#include <SFML/Graphics.hpp>
#include <mutex>
#include <vector>

#include "Console.h"
#include "events/EventBus.h"
#include "events/model/FieldChanged.h"
#include "misc/Config.h"
#include "misc/LambdaCreator.h"

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
      field_[i * Config::FIELD_HEIGHT + j].setPosition(
          {component_bounds_.getPosition().x + 6 + 18 * i,
           component_bounds_.getPosition().y + 4 + 18 * j});
    }
  }

  auto& event_bus = global::EventBus::get_instance();

  event_bus.subscribe<event::model::FieldChanged>(
      ::misc::LambdaCreator::create<&GameField::on_field_changed>(*this));
}

GameField::~GameField() {
  auto& event_bus = global::EventBus::get_instance();

  event_bus.unsubscribe<event::model::FieldChanged>(
      ::misc::LambdaCreator::create<&GameField::on_field_changed>(*this));
}

void GameField::draw() {
  std::unique_lock lock(mutex_);
  window_.draw(component_bounds_);
  for (auto& cell : field_) {
    window_.draw(cell);
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

}