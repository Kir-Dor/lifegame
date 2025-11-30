#pragma once

#include <SFML/Graphics.hpp>

namespace view::component {

struct IComponentCreateInfo {
  sf::RenderWindow& window;
  std::optional<sf::Event>& current_event;
  sf::Vector2f size;
  sf::Vector2f position;
};

class IComponent {
 public:
  IComponent(IComponentCreateInfo& create_info);
  virtual ~IComponent() = default;
  virtual void draw() = 0;
  virtual void on_sfml_event() = 0;

 protected:
  sf::RectangleShape component_bounds_;
  sf::Vector2f size_;
  sf::Vector2f position_;
  sf::RenderWindow& window_;
  std::optional<sf::Event>& current_event_;
};

}  // namespace view::component
