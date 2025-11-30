#include "InfoWindow.h"

#include "events/EventBus.h"
#include "events/other/Error.h"
#include "events/view/Resume.h"
#include "misc/LambdaCreator.h"

namespace view::component {
InfoWindow::InfoWindow(
    InfoWindowCreateInfo& create_info)
    : IComponent(create_info), font_(create_info.font), text_(font_) {
  component_bounds_.setFillColor(sf::Color::Black);
  component_bounds_.setOutlineThickness(3);
  component_bounds_.setOutlineColor(sf::Color::White);

  state_ = State::INACTIVE;

  text_.setCharacterSize(25);
  text_.setPosition({component_bounds_.getPosition().x + 5, component_bounds_.getPosition().y + 2});
  text_.setFillColor(sf::Color::White);

  auto& event_bus = global::EventBus::get_instance();

  event_bus.subscribe<event::Error, &InfoWindow::on_error>(*this);
}
InfoWindow::~InfoWindow() {
  auto& event_bus = global::EventBus::get_instance();

  event_bus.unsubscribe<event::Error, &InfoWindow::on_error>(*this);
}

void InfoWindow::draw() {
  switch (state_) {
    case State::ACTIVE : {
      window_.draw(component_bounds_);
      text_.setString(message_);
      window_.draw(text_);
    }
    default : {}
  }
}
void InfoWindow::on_sfml_event() {
  if (current_event_->is<sf::Event::KeyPressed>() && state_ == State::ACTIVE) {
    state_ = State::INACTIVE;
    global::EventBus::get_instance().invoke(event::view::Resume());
  }
}

void InfoWindow::on_error(const event::Error& error) {
  message_ = error.error_message;
  state_ = State::ACTIVE;
}

}