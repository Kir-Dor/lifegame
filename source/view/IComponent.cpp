#include "IComponent.h"

view::component::IComponent::IComponent(
    IComponentCreateInfo& create_info)
    : window_(create_info.window), current_event_(create_info.current_event), size_(create_info.size), position_(create_info.position) {
  component_bounds_.setSize(create_info.size);
  component_bounds_.setPosition(create_info.position);
}

