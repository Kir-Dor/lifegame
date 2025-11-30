#include "Core.h"

#include <memory>

#include "controller/GameController.h"
#include "events/EventBus.h"
#include "model/GameModel.h"
#include "view/GameView.h"
#include "view/IComponent.h"

void LifeGame::run() {
  global::Logger::get_instance();
  global::ThreadPool::get_instance();
  global::EventBus::get_instance();

  std::shared_ptr<model::GameModel> game_model = std::make_shared<model::GameModel>();
  std::shared_ptr<controller::GameController> game_contrfreoller = std::make_shared<controller::GameController>(game_model);
  std::shared_ptr<view::GameView> game_view = std::make_shared<view::GameView>();

  game_view->display();

  global::ThreadPool::get_instance().join();
}
