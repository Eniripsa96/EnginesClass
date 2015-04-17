#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "UIObject.h"
#include "Shaders.h"
#include "Camera.h"

using namespace std;

enum SCENE_TYPE
{
	MENU_SCENE,
	GAME_SCENE
};

class Scene
{
public:
	Scene();
	Scene(vector<GameObject*>*, SCENE_TYPE);
	~Scene();

	void Update(float);
	void Draw(void);

	void UpdateCameraOnResize(XMFLOAT4X4*);
	Camera* GetCamera(void) const;

	SCENE_TYPE type;

private:
	vector<GameObject*> gameObjects;

	Camera* camera;
};

#endif