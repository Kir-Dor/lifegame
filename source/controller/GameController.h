#pragma once
#include <atomic>
#include <memory>

namespace event::user {
class LoadFile;
class SaveFile;
class Exit;
class Tick;
class Start;
class Pause;
class Stop;
}

namespace model {
class GameModel;
}

namespace controller {

class GameController {
 public:
  GameController(const std::shared_ptr<model::GameModel>& game_model);
  ~GameController();

  void on_load(const event::user::LoadFile& event);
  void on_save(const event::user::SaveFile& event);
  void on_tick(const event::user::Tick& event);
  void on_start(const event::user::Start& event);
  void on_stop(const event::user::Stop& event);
  void on_pause(const event::user::Pause& event);
  void on_exit(const event::user::Exit& event);
 private:
  std::shared_ptr<model::GameModel> game_model_;
  std::atomic_bool start_flag_;
  std::atomic_bool stop_flag_;
  std::atomic_flag pause_flag_;
};

}  // namespace controller
