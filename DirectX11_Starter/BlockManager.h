#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H
#endif

#include "GameObject.h"
#include "ParticleSystem.h"

#include <stdlib.h>
#include <math.h>
#include <vector>

using namespace std;

// Values for the game
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
#define SLOW_FALL_SPEED 2.0f
#define FAST_FALL_SPEED 8.0f
#define SIDE_SPEED 5.0f
#define SPEED_INCREASE 0.01f
#define ROTATION_SPEED 12.0f
#define PI 3.1415926535f

// A block object in the game
struct Block
{
	GameObject* gameObject;
	bool threeByThree;
	bool* localGrid;
	bool* tempGrid;
	bool* grid;
};

// A direction to move a block
enum MoveDirection
{
	LEFT,
	RIGHT,
	DOWN
};

// Manages interactions with the blocks in the game
class BlockManager
{
public:
	BlockManager(Block* blocks, int numBlocks, vector<GameObject*> cubes, XMFLOAT3 min, XMFLOAT3 holdPos, float blockWidth, ParticleSystem* particleSystem);
	~BlockManager();
	
	void reset();
	void update(float dt);
	void draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData);

	bool canMove(MoveDirection direction);
	bool canOccupy(int x, int y);

	void spawnFallingBlock();
	void resetActiveBlock();
	void move(MoveDirection direction);
	void drop();
	void rotate();
	void holdBlock();
	void mergeBlock();
	void checkLines(int min, int max);
	XMFLOAT2 getGhostPos();
	bool isGameOver() { return gameOver; }
	int getScore() { return score; }

	float fallSpeed = SLOW_FALL_SPEED;

private:
	Block* blocks;
	bool* gameGrid;
	vector<GameObject*> cubes;
	int* typeOrder;
	int* scores;
	int score = 0;
	bool hacked = false;
	
	XMFLOAT3 min;
	XMFLOAT3 holdPos;
	float blockWidth;
	float rotation = 0;
	bool canSwap = true;
	bool gameOver = false;
	int numBlocks;
	int targetX;
	int targetY;
	int activeId = -1;
	int heldId = -1;

	void copy(bool* src, bool* dest, int num);
	void shuffle();

	ParticleSystem* particleSystem;
};

