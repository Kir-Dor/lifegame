#pragma once
#include <bitset>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace model {

class GameModel {
 public:
  GameModel();
  void load(const std::string& filepath);
  void save(std::string filename);

  void set(int x, int y, bool value);
  void next_generation();
  void clear();

  [[nodiscard]] std::vector<bool> get_data() const;

 private:
  std::mutex mutex_;

  bool initialized_;

  std::vector<bool> field_;
  std::vector<bool> temp_data_;

  std::bitset<9> rules_for_born_;
  std::bitset<9> rules_for_survive_;

  std::string universe_name_;
  
  uint8_t get_lives_around(int x, int y);
};

}  // namespace model
