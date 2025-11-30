#pragma once
#include <SFML/Graphics.hpp>

#include "events/ISubscriber.h"
#include "view/IComponent.h"

namespace event {
class Error;
}

namespace view::component {

struct InfoWindowCreateInfo : public IComponentCreateInfo {
  sf::Font font;
};

class InfoWindow final : public IComponent, public event::ISubscriber {
 public:
  InfoWindow(InfoWindowCreateInfo& create_info);
  ~InfoWindow();

  void draw() override;
  void on_sfml_event() override;

 private:
  enum class State {
    INACTIVE, ACTIVE
  };
  State state_;

  std::string message_;

  sf::Font font_;
  sf::Text text_;

  void on_error(const event::Error& error);
};

}  // namespace view::component