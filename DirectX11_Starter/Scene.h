#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "GameObject.h"
#include "Shaders.h"

using namespace std;

class Scene
{
public:
	Scene();
	Scene(vector<GameObject*>*);
	~Scene();

	void Update(float);
	void Draw(void);

//private:
	vector<GameObject*> gameObjects;
};

#endif