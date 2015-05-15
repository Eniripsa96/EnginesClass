#include "Judge.h"


Judge::Judge(Mesh* mesh, Material* mat, XMFLOAT3* pos, XMFLOAT3* vel)
	: GameObject(mesh, mat, pos, vel)
{
	int r = rand() % 256;
	int b = rand() % 256;
	int g = rand() % 256;

	favColor = XMFLOAT3((float)r, (float)g, (float)b);
	
	currentScore = 0;
}


Judge::~Judge()
{
}

void Judge::JudgeEffect(XMFLOAT3* color)
{
	float maxScoreFactor = 10.0f / 127.5f;	// Max score over max color value
	int score = (10 - (abs(color->x*255 - favColor.x) * maxScoreFactor)) + 0.5f;
	score = score < 0 ? 0 : score;

	if (score == 0)
		material = MeshesMaterials::materials["judge0"];
	else if (score == 1)
		material = MeshesMaterials::materials["judge1"];
	else if (score == 2)
		material = MeshesMaterials::materials["judge2"];
	else if (score == 3)
		material = MeshesMaterials::materials["judge3"];
	else if (score == 4)
		material = MeshesMaterials::materials["judge4"];
	else if (score == 5)
		material = MeshesMaterials::materials["judge5"];
	else if (score == 6)
		material = MeshesMaterials::materials["judge6"];
	else if (score == 7)
		material = MeshesMaterials::materials["judge7"];
	else if (score == 8)
		material = MeshesMaterials::materials["judge8"];
	else if (score == 9)
		material = MeshesMaterials::materials["judge9"];
	else
		material = MeshesMaterials::materials["judge10"];
}