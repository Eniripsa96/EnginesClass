#ifndef JUDGE_H
#define JUDGE_H

#include "GameObject.h"
#include "MeshesMaterials.h"

class Judge :
	public GameObject
{
public:
	Judge(Mesh*, Material*, XMFLOAT3*, XMFLOAT3*);
	~Judge();

	void JudgeEffect(XMFLOAT3*);

	XMFLOAT3 favColor;

private:
	
	int currentScore;
};

#endif