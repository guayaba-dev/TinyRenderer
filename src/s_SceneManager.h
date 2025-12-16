#pragma once

#include <fstream>

#include "l_scene.h"

bool loadScene(std::string sceneDir, Scene loadedScene);

bool saveScene(std::string sceneDir);
