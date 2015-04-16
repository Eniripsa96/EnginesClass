#include "Scene.h"


Scene::Scene()
{
}

Scene::Scene(vector<GameObject*>* objs, SCENE_TYPE type)
{
	gameObjects = *objs;
	this->type = type;
}

Scene::~Scene()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	gameObjects.clear();
}

void Scene::Update(float dt)
{
	for (UINT i = 0; i < gameObjects.size(); i++)
		gameObjects[i]->Update(dt);
}

void Scene::Draw()
{
	for (UINT i = 0; i < gameObjects.size(); i++)
		gameObjects[i]->Draw(Shaders::vsConstantBuffer, &Shaders::dataToSendToVSConstantBuffer);
}