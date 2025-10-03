#pragma once
#include <string>

#include "../.dependencies/entt/entt.hpp"
#include "l_matrix.h"
#include "model.h"

class Scene {
 private:
  entt::registry registry;

 public:
  Scene();
  ~Scene();

  void addModel(std::string src);
  auto& getRenderizable();
};
