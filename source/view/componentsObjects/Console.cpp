#include "Console.h"

#include "events/EventBus.h"
#include "events/other/Error.h"
#include "events/user/Clear.h"
#include "events/user/EditMode.h"
#include "events/view/Resume.h"
#include "misc/Config.h"
#include "misc/LambdaCreator.h"
#include "view/misc/CommandParser.h"

namespace view::component {
Console::Console(ConsoleCreateInfo& create_info)
    : IComponent(create_info), font_(create_info.font), text_(font_), fps_text_(font_), window_fps_(create_info.window_fps_) {
  state_ = State::ACTIVE;

  component_bounds_.setFillColor({64, 64, 64});

  text_.setCharacterSize(25);
  text_.setPosition(position_);
  text_.setFillColor(sf::Color::White);

  fps_text_.setCharacterSize(25);
  fps_text_.setFillColor(sf::Color::White);

  auto& event_bus = global::EventBus::get_instance();

  event_bus.subscribe<event::Error, &Console::on_error>(*this);
  event_bus.subscribe<event::view::Resume, &Console::on_resume>(*this);

  global::Logger::get_instance().log_debug(
      R"(View component "Console" initialized!)");
}

Console::~Console() {
  auto& event_bus = global::EventBus::get_instance();

  event_bus.unsubscribe<event::Error, &Console::on_error>(*this);
  event_bus.unsubscribe<event::view::Resume, &Console::on_resume>(*this);
}

void Console::draw() {
  window_.draw(component_bounds_);

  text_.setString(text_content_);
  window_.draw(text_);

  fps_string_ = fmt::format("FPS: {}", static_cast<int>(window_fps_));
  fps_text_.setPosition({position_.x + size_.x - fps_string_.length() * 13, position_.y});
  fps_text_.setString(fps_string_);
  window_.draw(fps_text_);
}

void Console::on_sfml_event() {
  switch (state_) {
    case State::ACTIVE: {
      text_content_ = "Enter Command ";
      if (edit_mode_) {
        text_content_ += "(Edit Mode) ";
      }
      text_content_ += ">> " + user_input;
      if (auto key_pressed = current_event_->getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->code == sf::Keyboard::Key::F3) {
          global::EventBus::get_instance().invoke(event::user::EditMode(!edit_mode_));
          edit_mode_ = !edit_mode_;
        }
      }
      if (auto text_entered = current_event_->getIf<sf::Event::TextEntered>()) {

        global::Logger::get_instance().log_trace(
            "Input symbol with unicode: {}",
            static_cast<uint32_t>(text_entered->unicode));

        switch (text_entered->unicode) {
          case 3: // ctrl + C
          case 27: { // esc
            window_.close();
            return;
          }
          case 22: { // ctrl + v
            std::string clipboard = sf::Clipboard::getString();
            user_input += clipboard;
            break;
          }
          case 8: { // backspace
            if (!user_input.empty()) {
              user_input.pop_back();
            }
            break;
          }
          case 127: { // cntr + backspace
            constexpr std::string word_separators = " ";
            auto last_word_index = user_input.find_last_of(word_separators);
            if (last_word_index == std::string::npos) {
              user_input = "";
            }
            else {
              user_input.erase(user_input.begin() + last_word_index, user_input.end());
            }
            return;
          }
          case 13: { // enter
            view::misc::CommandParser::ParseAndInvoke(user_input);
            user_input = "";
            break;
          }
          default: {
            user_input += static_cast<char>(text_entered->unicode);
            break;
          }
        }
      }
      break;
    }
    case State::WAITING: {
      state_ = State::ACTIVE;
      text_content_ = "";
      break;
    }
    default: {
      text_content_ = "INACTIVE";
    }
  }
}

void Console::on_error(const event::Error& error) { state_ = State::INACTIVE; }

void Console::on_resume(const event::view::Resume& resume) {
  state_ = State::WAITING;
}
}  // namespace view::component