#include "Scene.h"


Scene::Scene()
{
}

Scene::Scene(vector<GameObject*>* objs, SCENE_TYPE type)
{
	gameObjects = *objs;
	this->type = type;

	camera = new Camera();
}

Scene::~Scene()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	gameObjects.clear();

	delete camera;
}

void Scene::Update(float dt)
{
	// Update game objects in scene
	for (UINT i = 0; i < gameObjects.size(); i++)
		gameObjects[i]->Update(dt);
}

void Scene::Draw()
{
	for (UINT i = 0; i < gameObjects.size(); i++)
		gameObjects[i]->Draw(Shaders::vsConstantBuffer, &Shaders::dataToSendToVSConstantBuffer);
}

void Scene::UpdateCameraOnResize(XMFLOAT4X4* proj)
{
	camera->projectionMatrix = *proj;
}

Camera* Scene::GetCamera() const
{
	return camera;
}