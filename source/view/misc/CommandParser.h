#pragma once
#include <string>

namespace view::misc {

class CommandParser {
public:
  static void ParseAndInvoke(std::string command);
};

}