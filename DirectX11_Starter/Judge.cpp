#include "Judge.h"


Judge::Judge(Mesh* mesh, Material* mat, XMFLOAT3* pos, XMFLOAT3* vel)
	: GameObject(mesh, mat, pos, vel)
{
	// Increase the size of all judges
	Scale(&XMFLOAT3(2.0f, 2.0f, 2.0f));

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
	// Create factor to put scores between 0-10
	float maxScoreFactor = 10.0f / 127.5f;	// Max score over max color value

	// Calculate component scores based on favorite color
	int redScore = (10 - (abs(color->x*255 - favColor.x) * maxScoreFactor)) + 0.5f;
	int greenScore = (10 - (abs(color->y * 255 - favColor.y) * maxScoreFactor)) + 0.5f;
	int blueScore = (10 - (abs(color->z * 255 - favColor.z) * maxScoreFactor)) + 0.5f;
	// Fix the edge-case where scores are -2 which is an invalid score
	redScore = redScore < 0 ? 0 : redScore;
	greenScore = greenScore < 0 ? 0 : greenScore;
	blueScore = blueScore < 0 ? 0 : blueScore;

	// Compile the color scores into an overall score
	int score = (redScore + greenScore + blueScore) / 3.0f + 0.5f;

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