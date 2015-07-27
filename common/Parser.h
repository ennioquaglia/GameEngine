#pragma once

#include <string>
#include <map>
#include "Debug.h"
/*

Parse file into 3D scene

Implemented format:
-none

TODO
-Collada



*/
namespace Engine{
	class Scene;
}

bool loadScene(std::string filename,Engine::Scene* scene);