#pragma once

#include "core.h"
#include "camera.h"
#include "model.h"
using namespace  Math;

class Scene
{
public:
	Scene() = default;
	~Scene() = default;
	
	virtual void GenerateScene() = 0;

private:
	std::shared_ptr<Camera>  m_camera;
	std::vector<std::shared_ptr<Model>> m_models;
};

class Scene_1 : public Scene 
{
public:
	Scene_1() = default;
	virtual void GenerateScene();
};

