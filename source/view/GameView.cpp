#include "GameView.h"

#include "events/EventBus.h"
#include "events/user/Exit.h"
#include "misc/Config.h"
#include "misc/ThreadPool.h"
#include "view/IComponent.h"
#include "view/componentsObjects/Console.h"
#include "view/componentsObjects/GameField.h"
#include "view/componentsObjects/InfoWindow.h"

namespace view {
void GameView::display() {
  initialize();

  while (window_.isOpen()) {
    auto time_start = std::chrono::high_resolution_clock::now();
    current_window_event_ = window_.pollEvent();
    if (current_window_event_.has_value()) {
      if (current_window_event_->is<sf::Event::Closed>()) {
        window_.close();
        break;
      }
      for (auto& component : components_) {
        component->on_sfml_event();
      }
    }

    window_.clear(sf::Color::Black);

    for (auto& component : components_) {
      component->draw();
    }

    window_.display();
    auto time_end = std::chrono::high_resolution_clock::now();
    frame_duration_ = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
    fps_update();
  }

  global::EventBus::get_instance().invoke(event::user::Exit());
}

void GameView::initialize() {
  window_.create(sf::VideoMode::getDesktopMode(), "test", sf::Style::Default,
                 sf::State::Fullscreen);

  global::Logger::get_instance().log_debug("Window created!");

  component::ConsoleCreateInfo console_create_info{
      window_,
      current_window_event_,
      {static_cast<float>(window_.getSize().x), 30},
      {0, 0},
      sf::Font("assets/fonts/Roboto-Light.ttf"),
      fps_};

  components_.push_back(
      std::make_unique<component::Console>(console_create_info));

  component::GameFieldCreateInfo game_field_create_info{
      window_,
      current_window_event_,
      {static_cast<float>(window_.getSize().x),
       static_cast<float>(window_.getSize().y - 30)},
      {0, 30}};

  components_.push_back(
      std::make_unique<component::GameField>(game_field_create_info));

  component::InfoWindowCreateInfo info_window_create_info{
      window_,
      current_window_event_,
      {static_cast<float>(window_.getSize().x / 2),
       static_cast<float>(window_.getSize().y / 2)},
      {static_cast<float>(window_.getSize().x / 4),
       static_cast<float>(window_.getSize().y / 4)},
      sf::Font("assets/fonts/Roboto-Light.ttf")};

  components_.push_back(
      std::make_unique<component::InfoWindow>(info_window_create_info));

  global::Logger::get_instance().log_debug("View initialized!");
}

void GameView::fps_update() {
  fps_tick_duration -= frame_duration_;
  fps_sum_ += 1'000'000.0f / frame_duration_.count();
  sum_counter_++;
  if (fps_tick_duration <= std::chrono::microseconds(0)) {
    fps_ = fps_sum_ / sum_counter_;
    fps_sum_ = 0;
    sum_counter_ = 0;
    fps_tick_duration = std::chrono::microseconds(static_cast<int>(1'000'000.0f / Config::FPS_TICK_RATE));
  }
}

}  // namespace view