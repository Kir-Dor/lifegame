#include "GameModel.h"
#include "misc/Config.h"
#include "events/EventBus.h"
#include "events/model/FieldChanged.h"
#include "events/model/CellChanged.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>

namespace model {

GameModel::GameModel() {
  rules_for_born_ = 0;
  rules_for_survive_ = 0;
  global::Logger::get_instance().log_debug("Model initialized!");
}

void GameModel::load(const std::string& filepath) {
  global::Logger::get_instance().log_debug("GameModel::load(\"{}\")", filepath);
  clear();
  std::unique_lock lock(mutex_);

  boost::filesystem::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file");
  }
  std::string line;

  std::getline(file, line);
  if (line.substr(0, 2) != "#N") {
    throw std::runtime_error("Wrong input file");
  }
  line = line.substr(2);
  boost::trim(line);

  universe_name_ = line;

  std::getline(file, line);
  if (line.substr(0, 2) != "#R") {
    throw std::runtime_error("Wrong input file");
  }
  line = line.substr(2);
  boost::trim(line);
  if (line[0] != 'B') {
    throw std::runtime_error("Wrong input file");
  }
  line = line.substr(1);
  auto slash_pos = line.find('/');
  if (slash_pos == std::string::npos) {
    throw std::runtime_error("Wrong input file");
  }
  for (auto i = 0; i < slash_pos; ++i) {
    rules_for_born_[line[i] - '0'] = true;
  }
  line = line.substr(slash_pos + 1);
  boost::trim(line);
  if (line[0] != 'S') {
    throw std::runtime_error("Wrong input file");
  }
  line = line.substr(1);
  for (auto i = 0; i < line.length(); ++i) {
    rules_for_survive_[line[i] - '0'] = true;
  }

  field_.resize(Config::FIELD_WIDTH * Config::FIELD_HEIGHT, false);
  temp_data_.resize(Config::FIELD_WIDTH * Config::FIELD_HEIGHT, false);

  while (std::getline(file, line)) {
    auto space_pos = line.find(' ');
    if (space_pos == std::string::npos) {
      throw std::runtime_error("Wrong input file");
    }
    auto x = std::stoi(line.substr(0, space_pos));
    auto y = std::stoi(line.substr(space_pos + 1));
    field_[x * Config::FIELD_HEIGHT + y] = true;
  }

  initialized_ = true;
  global::EventBus::get_instance().invoke(event::model::FieldChanged(field_));
}

void GameModel::save(std::string filename) {
  global::Logger::get_instance().log_debug("GameModel::save(\"{}\")", filename);
  std::unique_lock lock(mutex_);

  boost::trim(filename);
  boost::filesystem::ofstream file("saves/" + filename + ".life");
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file");
  }
  if (field_.empty()) {
    throw std::runtime_error("Not loaded field");
  }

  file << "#N " << universe_name_ << "\n";

  file << "#R B";
  for (auto i = 0; i < 9; ++i) {
    if (rules_for_born_[i]) {
      file << i;
    }
  }
  file << "/S";
  for (auto i = 0; i < 9; ++i) {
    if (rules_for_survive_[i]) {
      file << i;
    }
  }
  file << '\n';

  for (auto i = 0; i < Config::FIELD_WIDTH; ++i) {
    for (auto j = 0; j < Config::FIELD_HEIGHT; ++j) {
      if (field_[i * Config::FIELD_HEIGHT + j]) {
        file << i << ' ' << j << '\n';
      }
    }
  }
}
void GameModel::set(int x, int y, bool value) {
  global::Logger::get_instance().log_debug("GameModel::set({},{},{})", x, y, value);
  std::unique_lock lock(mutex_);
  if (!initialized_) {
    throw std::runtime_error("Try set value to uninitialized field");
  }
  field_[x * Config::FIELD_HEIGHT + y] = value;
  global::EventBus::get_instance().invoke(event::model::CellChanged(x, y, value));
}

void GameModel::next_generation() {
  global::Logger::get_instance().log_debug("GameModel::next_generation()");
  std::unique_lock lock(mutex_);
  auto time_start = std::chrono::high_resolution_clock::now();
  if (field_.empty()) {
    throw std::runtime_error("Not loaded field");
  }
  for (auto i = 0; i < Config::FIELD_WIDTH; ++i) {
    for (auto j = 0; j < Config::FIELD_HEIGHT; ++j) {
      uint8_t lives_around = get_lives_around(i, j);
      if (field_[i * Config::FIELD_HEIGHT + j]) {
        if (rules_for_survive_[lives_around]) {
          temp_data_[i * Config::FIELD_HEIGHT + j] = true;
        }
        else {
          temp_data_[i * Config::FIELD_HEIGHT + j] = false;
        }
      }
      else {
        if (rules_for_born_[lives_around]) {
          temp_data_[i * Config::FIELD_HEIGHT + j] = true;
        }
        else {
          temp_data_[i * Config::FIELD_HEIGHT + j] = false;
        }
      }
    }
  }
  std::swap(field_, temp_data_);
  global::EventBus::get_instance().invoke(event::model::FieldChanged(field_));
  auto time_end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
  auto sleep_duration = std::chrono::microseconds(1'000'000) / Config::GAME_TICK_RATE - duration;
  if (sleep_duration < std::chrono::microseconds(0)) {
    sleep_duration = std::chrono::microseconds(0);
  }
  std::this_thread::sleep_for(sleep_duration);
}

void GameModel::clear() {
  global::Logger::get_instance().log_debug("GameModel::clear()");
  std::unique_lock lock(mutex_);
  std::fill(field_.begin(), field_.end(), false);
  global::EventBus::get_instance().invoke(event::model::FieldChanged(field_));
}

std::vector<bool> GameModel::get_data() const {
  global::Logger::get_instance().log_debug("GameModel::get_data()");
  return field_;
}

uint8_t GameModel::get_lives_around(int x, int y) {
  uint8_t lives_around = 0;
  int prev_x = x == 0 ? Config::FIELD_WIDTH - 1 : x - 1;
  int prev_y = y == 0 ? Config::FIELD_HEIGHT - 1 : y - 1;
  int next_x = x == Config::FIELD_WIDTH - 1 ? 0 : x + 1;
  int next_y = y == Config::FIELD_HEIGHT - 1 ? 0 : y + 1;

  lives_around += field_[prev_x * Config::FIELD_HEIGHT + prev_y];
  lives_around += field_[prev_x * Config::FIELD_HEIGHT + y];
  lives_around += field_[prev_x * Config::FIELD_HEIGHT + next_y];
  lives_around += field_[x * Config::FIELD_HEIGHT + prev_y];
  lives_around += field_[x * Config::FIELD_HEIGHT + next_y];
  lives_around += field_[next_x * Config::FIELD_HEIGHT + prev_y];
  lives_around += field_[next_x * Config::FIELD_HEIGHT + y];
  lives_around += field_[next_x * Config::FIELD_HEIGHT + next_y];

  return lives_around;
}

}