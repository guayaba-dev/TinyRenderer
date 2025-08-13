#include "s_SceneManager.h"

#include <fstream>

class Scene {};

bool loadScene(std::string sceneDir) {
  std::ifstream file(sceneDir);

  if (!file.is_open()) return false;

  return true;
}

bool saveScene(std::string sceneDir) {
  std::ofstream file(sceneDir);

  if (!file.is_open()) return false;

  return true;
};
