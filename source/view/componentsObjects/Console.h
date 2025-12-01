#pragma once
#include <SFML/Graphics.hpp>
#include <atomic>

#include "events/ISubscriber.h"
#include "view/IComponent.h"

namespace event {
class Error;
}

namespace event::view {
class Resume;
}

namespace view::component {
struct ConsoleCreateInfo : public IComponentCreateInfo {
  sf::Font font;
  float& window_fps_;
};

class Console final : public IComponent, public event::ISubscriber {
public:
  Console(ConsoleCreateInfo& create_info);
  ~Console();

  void draw() override;
  void on_sfml_event() override;
private:
  enum class State {
    ACTIVE, INACTIVE, WAITING
  };
  std::atomic<State> state_;
  std::atomic_bool edit_mode_;

  std::string text_content_;
  std::string user_input;

  sf::Font font_;
  sf::Text text_;

  float& window_fps_;
  std::string fps_string_;
  sf::Text fps_text_;

  void on_error(const event::Error& error);
  void on_resume(const event::view::Resume& resume);
};

}  // namespace view::component