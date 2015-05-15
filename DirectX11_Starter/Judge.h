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

private:
	XMFLOAT3 favColor;

	int currentScore;
};

#endif