#pragma once

namespace event {

class ISubscriber {};

template <typename T>
concept SubscribeConcept = requires {
  std::is_base_of_v<ISubscriber, T>;
};

}