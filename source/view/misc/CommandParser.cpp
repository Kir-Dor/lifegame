#include "CommandParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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

namespace view::misc {

void CommandParser::ParseAndInvoke(std::string command) {
  boost::trim(command);
  auto& event_bus = global::EventBus::get_instance();

  if (command.empty()) {
    return;
  }

  if (command == "exit") {
    event_bus.invoke(event::user::Exit());
    return;
  }

  if (command == "start") {
    event_bus.invoke(event::user::Start());
  }

  if (command == "stop") {
    event_bus.invoke(event::user::Stop());
  }

  if (command == "pause") {
    event_bus.invoke(event::user::Pause());
  }

  if (command == "clear") {
    event_bus.invoke(event::user::Clear());
  }

  if (boost::starts_with(command, "load")) try {
    command = command.substr(4);
    boost::trim(command);
    event_bus.invoke(event::user::LoadFile(command));
    return;
  } catch (std::exception& e) {
    event_bus.invoke(event::Error(e.what()));
    return;
  }

  if (boost::starts_with(command, "save")) try {
    command = command.substr(4);
    boost::trim(command);
    event_bus.invoke(event::user::SaveFile(command));
    return;
  } catch (std::exception& e) {
    event_bus.invoke(event::Error(e.what()));
    return;
  }

  if (boost::starts_with(command, "tick")) try {
    command = command.substr(4);
    boost::trim(command);
    size_t iteration_count = boost::lexical_cast<size_t>(command);
    event_bus.invoke(event::user::Tick(iteration_count));
    return;
  } catch (std::exception& e) {
    event_bus.invoke(event::Error(e.what()));
    return;
  }
}


}
