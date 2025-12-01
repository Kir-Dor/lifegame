#include "GameController.h"

#include "events/EventBus.h"
#include "events/other/Error.h"
#include "events/user/Clear.h"
#include "events/user/Exit.h"
#include "events/user/LoadFile.h"
#include "events/user/Pause.h"
#include "events/user/SaveFile.h"
#include "events/user/Start.h"
#include "events/user/Stop.h"
#include "events/user/Tick.h"
#include "events/view/CellChanged.h"
#include "model/GameModel.h"

namespace controller {

GameController::GameController(
    const std::shared_ptr<model::GameModel>& game_model)
    : game_model_(game_model) {
  stop_flag_ = false;

  auto& event_bus = global::EventBus::get_instance();
  auto& logger = global::Logger::get_instance();

  event_bus.subscribe<event::user::LoadFile, &GameController::on_load>(*this);
  event_bus.subscribe<event::user::SaveFile, &GameController::on_save>(*this);
  event_bus.subscribe<event::user::Tick, &GameController::on_tick>(*this);
  event_bus.subscribe<event::user::Start, &GameController::on_start>(*this);
  event_bus.subscribe<event::user::Stop, &GameController::on_stop>(*this);
  event_bus.subscribe<event::user::Pause, &GameController::on_pause>(*this);
  event_bus.subscribe<event::user::Exit, &GameController::on_exit>(*this);
  event_bus.subscribe<event::user::Clear, &GameController::on_clear>(*this);
  event_bus.subscribe<event::view::CellChanged, &GameController::on_cell_changed>(*this);

  logger.log_debug("Controller initialized!");
}

GameController::~GameController() {
  auto& event_bus = global::EventBus::get_instance();

  event_bus.unsubscribe<event::user::LoadFile, &GameController::on_load>(*this);
  event_bus.unsubscribe<event::user::SaveFile, &GameController::on_save>(*this);
  event_bus.unsubscribe<event::user::Tick, &GameController::on_tick>(*this);
  event_bus.unsubscribe<event::user::Start, &GameController::on_start>(*this);
  event_bus.unsubscribe<event::user::Stop, &GameController::on_stop>(*this);
  event_bus.unsubscribe<event::user::Pause, &GameController::on_pause>(*this);
  event_bus.unsubscribe<event::user::Exit, &GameController::on_exit>(*this);
  event_bus.unsubscribe<event::user::Clear, &GameController::on_clear>(*this);
  event_bus.unsubscribe<event::view::CellChanged, &GameController::on_cell_changed>(*this);
}

void GameController::on_load(const event::user::LoadFile& event) {
  if (start_flag_) {
    global::EventBus::get_instance().invoke(event::user::Stop());
    while (!start_flag_) {}
  }
  try {
    game_model_->load(event.file_path);
  } catch (std::exception& e) {
    global::EventBus::get_instance().invoke(event::Error(e.what()));
  }
}

void GameController::on_save(const event::user::SaveFile& event) {
  try {
    game_model_->save(event.file_name);
  } catch (std::exception& e) {
    global::EventBus::get_instance().invoke(event::Error(e.what()));
  }
}

void GameController::on_exit(const event::user::Exit& event) {
  stop_flag_ = true;
}

void GameController::on_clear(const event::user::Clear& event) {
  if (start_flag_) {
    global::EventBus::get_instance().invoke(event::user::Stop());
    while (!start_flag_) {}
  }
  try {
    game_model_->clear();
  } catch (std::exception& e) {
    global::EventBus::get_instance().invoke(event::Error(e.what()));
  }
}

void GameController::on_cell_changed(const event::view::CellChanged& event) {
  try {
    game_model_->set(event.x, event.y, event.value);
  } catch (std::exception& e) {
    global::EventBus::get_instance().invoke(event::Error(e.what()));
  }
}

void GameController::on_tick(const event::user::Tick& event) {
  try {
    if (start_flag_) {
      return;
    }
    start_flag_ = true;
    stop_flag_ = false;
    for (auto i = 0; i < event.iteration_count; ++i) {
      if (pause_flag_.test()) {
        pause_flag_.wait(true);
      }
      if (stop_flag_) {
        start_flag_ = false;
        break;
      }
      game_model_->next_generation();
    }
    start_flag_ = false;
  } catch (std::exception& e) {
    global::EventBus::get_instance().invoke(event::Error(e.what()));
  }
}

void GameController::on_start(const event::user::Start& event) {
  try {
    if (start_flag_) {
      pause_flag_.clear();
      pause_flag_.notify_all();
      return;
    }
    start_flag_ = true;
    stop_flag_ = false;
    while (!stop_flag_) {
      game_model_->next_generation();
      if (pause_flag_.test()) {
        pause_flag_.wait(true);
      }
    }
    start_flag_ = false;
  } catch (std::exception& e) {
    global::EventBus::get_instance().invoke(event::Error(e.what()));
  }
}

void GameController::on_stop(const event::user::Stop& event) {
  stop_flag_ = true;
  pause_flag_.clear();
  pause_flag_.notify_all();
}

void GameController::on_pause(const event::user::Pause& event) {
  pause_flag_.test_and_set();
}

}  // namespace controller