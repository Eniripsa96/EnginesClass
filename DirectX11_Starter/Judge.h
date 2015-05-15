#pragma once
#include "GameObject.h"
class Judge :
	public GameObject
{
public:
	Judge(Mesh*, Material*, XMFLOAT3*, XMFLOAT3*);
	~Judge();
};

