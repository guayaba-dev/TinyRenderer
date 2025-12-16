#pragma once
#include <string>

#include "../.dependencies/entt/entt.hpp"
#include "l_matrix.h"
#include "l_shaderDefinition.h"
#include "model.h"

struct Transform {
  Vec3f pos;
  // TODO: Add quad Rotation
  Vec3f scale;
  matrix<4, 4> world_matrix;
};

struct mesh {
  Model* model;
};  // TODO: add model components or change model to separate the data

struct material {
  IShader* shader;
};

struct camera {
  Vec3f center, pos, UP;
};

class Scene {
 private:
  entt::registry s_registry;

 public:
  Scene();
  ~Scene();

  entt::registry& getRegistry() { return s_registry; };
};
