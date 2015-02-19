#include "BlockManager.h"

// Initializes the BlockManager given 
// the minimum coordinates for blocks, the hold position for blocks, 
// and the width of each block
BlockManager::BlockManager(Block* pBlocks, int pNumBlocks, vector<GameObject*> pCubes, XMFLOAT3 pMin, XMFLOAT3 pHoldPos, float pBlockWidth, ParticleSystem* particleSystem)
{
	this->particleSystem = particleSystem;

	blocks = pBlocks;
	numBlocks = pNumBlocks;
	cubes = pCubes;
	min = pMin;
	holdPos = pHoldPos;
	blockWidth = pBlockWidth;
	gameGrid = new bool[GRID_WIDTH * GRID_HEIGHT];
	typeOrder = new int[numBlocks];
	scores = new int[4] { 40, 100, 300, 1200 };
	reset();
}

// Clears the pointers used by the block manager on deconstruct
BlockManager::~BlockManager()
{
	delete[] gameGrid;
	delete[] typeOrder;
	delete[] scores;
}

// Resets the game for a new one
void BlockManager::reset()
{
	activeId = 0;
	gameOver = false;
	for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
	{
		gameGrid[i] = false;
	}
	shuffle();
}

// Updates the blocks in the game
void BlockManager::update(float dt) 
{
	if (!hacked)
	{
		hacked = true;
		mergeBlock();
	}

	// Cannot update if the game is not active
	if (activeId == -1) {
		return;
	}

	// Convert block position to grid position
	XMFLOAT3 pos = blocks[typeOrder[activeId]].gameObject->position;
	float x = (pos.x / blockWidth) - min.x;
	float y = (pos.y / blockWidth) - min.y;

	// Apply smooth horizontal movement
	float dx = targetX - x;
	if (dx > 0)
	{
		pos.x += blockWidth * min(dx, dt * SIDE_SPEED);
	}
	else
	{
		pos.x += blockWidth * max(dx, -dt * SIDE_SPEED);
	}

	// Apply "gravity"
	float dy = targetY - y;
	float speed = -fallSpeed * dt;
	if (dy >= speed && canMove(DOWN))
	{
		targetY--;
		dy = targetY - y;
	}

	// Apply smooth vertical movement
	float yChange = max(dy, speed);
	if (yChange == 0 && targetX == x && rotation == 0)
	{
		mergeBlock();
		return;
	}
	else
	{
		pos.y += yChange * blockWidth;
	}
	blocks[typeOrder[activeId]].gameObject->position = pos;

	// Apply smooth rotation
	if (rotation > 0)
	{
		float angle = min(rotation, dt * ROTATION_SPEED);
		blocks[typeOrder[activeId]].gameObject->Rotate(&XMFLOAT3(0, 0, -angle));
		rotation -= angle;
	}
}

// Draws the blocks in the game
void BlockManager::draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData)
{
	cBufferData->lightDirection = XMFLOAT4(2.0f, -3.0f, 1.0f, 0.25f);
	cBufferData->color.w = 0.7f;

	// Active block
	if (activeId != -1) {
		blocks[typeOrder[activeId]].gameObject->Update(0);
		blocks[typeOrder[activeId]].gameObject->Draw(deviceContext, cBuffer, cBufferData);
	}

	// Grid cubes
	for (int i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++) {
		if (gameGrid[i]) {
			cubes[i]->Update(0);
			cubes[i]->Draw(deviceContext, cBuffer, cBufferData);
		}
	}

	// Held block
	if (heldId != -1)
	{
		XMFLOAT3 prev = blocks[typeOrder[heldId]].gameObject->position;
		float halfSize = blocks[typeOrder[heldId]].threeByThree ? 1.5f : 2.0f;
		blocks[typeOrder[heldId]].gameObject->position = XMFLOAT3(holdPos.x - halfSize, holdPos.y - halfSize, min.z);
		float rotation = blocks[typeOrder[heldId]].gameObject->rotation.z;
		blocks[typeOrder[heldId]].gameObject->rotation.z = 0;

		blocks[typeOrder[heldId]].gameObject->Update(0);
		blocks[typeOrder[heldId]].gameObject->Draw(deviceContext, cBuffer, cBufferData);
		blocks[typeOrder[heldId]].gameObject->position = prev;
		blocks[typeOrder[heldId]].gameObject->rotation.z = rotation;
	}

	// Ghost block
	if (activeId != -1) 
	{
		cBufferData->color.w = 0.3f;

		XMFLOAT3 prev = blocks[typeOrder[activeId]].gameObject->position;
		blocks[typeOrder[activeId]].gameObject->position = XMFLOAT3(prev.x, getGhostPos().y, prev.z);
		blocks[typeOrder[activeId]].gameObject->Update(0);
		blocks[typeOrder[activeId]].gameObject->Draw(deviceContext, cBuffer, cBufferData);
		blocks[typeOrder[activeId]].gameObject->position = prev;
	}

	// Restore the transparency
	cBufferData->color.w = 1;
}

// Checks whether or not a move in the given direction can be made
bool BlockManager::canMove(MoveDirection direction)
{
	// If the game is not active, no moves can be made
	if (activeId == -1) {
		return false;
	}

	// Check for collisions when active
	switch (direction)
	{
	case LEFT:
		return canOccupy(targetX - 1, targetY);
	case RIGHT:
		return canOccupy(targetX + 1, targetY);
	case DOWN:
		return canOccupy(targetX, targetY - 1);
	default:
		return false;
	}
}

// Checks whether or not the active block can fit into the given spot
// with it's current orientation
bool BlockManager::canOccupy(int x, int y)
{
	// Cannot check if the game is not active
	if (activeId == -1) {
		return false;
	}

	// Compare the active block's local grid with the game grid at the position
	int size = blocks[typeOrder[activeId]].threeByThree ? 3 : 4;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			// Ignore empty cells of the block
			if (!blocks[typeOrder[activeId]].localGrid[i + j * size])
			{
				continue;
			}

			// Limit it to the game board
			else if (x + i < 0 || x + i >= GRID_WIDTH || y + j < 0)
			{
				return false;
			}

			// Ignore above the board
			else if (y + j >= GRID_HEIGHT)
			{
				continue;
			}

			// Check for collision with other blocks
			else if (gameGrid[x + i + (y + j) * GRID_WIDTH] != NULL)
			{
				return false;
			}
		}
	}

	// No collisions
	return true;
}

// Spawns a new falling block at the top of the game
void BlockManager::spawnFallingBlock() 
{
	// Get a random block
	activeId = (activeId + 1) % numBlocks;
	if (activeId == 0)
	{
		shuffle();
	}
	resetActiveBlock();
}

// Resets the active block to the top of the game
void BlockManager::resetActiveBlock()
{
	Block block = blocks[typeOrder[activeId]];

	// Reset target position
	targetY = GRID_HEIGHT;
	targetX = GRID_WIDTH / 2 - 2;

	// Get the spawn location
	float x = blockWidth * targetX + min.x;
	float y = blockWidth * targetY + min.y;
	float z = min.z;

	// Set up the block
	int size = block.threeByThree ? 9 : 16;
	block.gameObject->position = XMFLOAT3(x, y, z);
	block.gameObject->ClearRotation();
	copy(block.grid, block.tempGrid, size);
	copy(block.grid, block.localGrid, size);
}

// Tries to move the active block in the given direction
void BlockManager::move(MoveDirection direction)
{
	// Cannot move if the game is not active
	if (activeId == -1) {
		return;
	}

	// Convert block position to grid position
	float x = (blocks[typeOrder[activeId]].gameObject->position.x / blockWidth) - min.x;

	// Ignore it if it's still mid-movement
	float dx = abs(x - targetX);
	if (dx > 0.1f || !canMove(direction)) return;

	// Apply the move
	switch (direction)
	{
	case LEFT:
		targetX--;
		break;
	case RIGHT:
		targetX++;
		break;
	}
}

// Instantly drops the active block
void BlockManager::drop()
{
	if (activeId >= 0)
	{
		targetY = getGhostPos().x;
		XMFLOAT3 pos = blocks[typeOrder[activeId]].gameObject->position;
		blocks[typeOrder[activeId]].gameObject->Move(&XMFLOAT3(targetX * blockWidth + min.x - pos.x, targetY * blockWidth + min.y - pos.y, 0));
	}
}

// Rotates the active block if able to
void BlockManager::rotate()
{
	// Cannot rotate if the game is not active
	if (activeId == -1 || rotation > 0) {
		return;
	}

	// Rotate the temp grid into the local grid
	int size = blocks[typeOrder[activeId]].threeByThree ? 3 : 4;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			blocks[typeOrder[activeId]].localGrid[i + j * size] = blocks[typeOrder[activeId]].tempGrid[(size - 1 - j) + i * size];
		}
	}

	// See if the rotation is valid
	bool canRotateNormal = canOccupy(targetX, targetY);
	bool canRotateRight = !canRotateNormal && canOccupy(targetX + 1, targetY);
	bool canRotateLeft = !canRotateNormal && canOccupy(targetX - 1, targetY);
	if (canRotateNormal || canRotateLeft || canRotateRight)
	{
		// Move if necessary
		if (canRotateRight)
		{
			targetX++;
		}
		else if (canRotateLeft)
		{
			targetX--;
		}

		// Update the temp grid
		rotation += PI / 2;
		copy(blocks[typeOrder[activeId]].localGrid, blocks[typeOrder[activeId]].tempGrid, size * size);
	}

	// Restore the local grid if it cannot rotate
	else
	{
		copy(blocks[typeOrder[activeId]].tempGrid, blocks[typeOrder[activeId]].localGrid, size * size);
	}
}

// Moves the active block to the held spot and replaces it with a new block
// or the previously held block
void BlockManager::holdBlock()
{
	// Cannot hold a block if the game is not active
	if (activeId == -1) {
		return;
	}

	if (canSwap)
	{
		canSwap = false;
		int held = heldId;
		heldId = activeId;

		// If there is no held block, spawn a new one
		if (held == -1)
		{
			spawnFallingBlock();
		}

		// Otherwise grab the falling block
		else
		{
			activeId = held;
			resetActiveBlock();
		}
	}
}

// Merges the active block into the game grid
void BlockManager::mergeBlock()
{
	// Cannot merge a block if the game is not active
	if (activeId == -1) {
		return;
	}

	canSwap = true;

	int minY = GRID_HEIGHT;
	int maxY = 0;
	int size = blocks[typeOrder[activeId]].threeByThree ? 3 : 4;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			bool cell = blocks[typeOrder[activeId]].localGrid[i + j * size];
			if (cell && cell != 205)
			{
				// Game over
				if (targetY + j >= GRID_HEIGHT || gameGrid[i + targetX + (j + targetY) * GRID_WIDTH] != NULL)  
				{
					gameOver = true;
					activeId = -1;
					return;
				}

				// Mark the min/max rows for checking lines
				if (targetY + j < minY)
				{
					minY = targetY + j;
				}
				if (targetY + j > maxY)
				{
					maxY = targetY + j;
				}

				// Merge the cell
				float x = (targetX + i) * blockWidth + min.x;
				float y = (targetY + j) * blockWidth + min.y;
				float z = min.z;
				cubes[targetX + i + (targetY + j) * GRID_WIDTH]->material = blocks[typeOrder[activeId]].gameObject->material;
				gameGrid[targetX + i + (targetY + j) * GRID_WIDTH] = true;
			}
		}
	}

	// Check lines for completion
	checkLines(minY, maxY);

	// Spawn a new block
	spawnFallingBlock();
}

// Checks the game grid for cleared lines
void BlockManager::checkLines(int min, int max)
{
	int cleared = 0;
	for (int i = max; i >= min; i--)
	{
		// Check each individual line for completion
		bool complete = true;
		for (int j = 0; j < GRID_WIDTH; j++)
		{
			complete &= (gameGrid[j + i * GRID_WIDTH] != NULL);
		}

		// Clear the line and move down above lines if complete
		if (complete)
		{
			cleared++;

			// TODO change our gameobjects to an effect if desired
			particleSystem->Reset();

			// Delete the game objects for now
			for (int j = 0; j < GRID_WIDTH; j++)
			{
				gameGrid[j + i * GRID_WIDTH] = false;
			}

			// Move down higher rows
			for (int j = i; j < GRID_HEIGHT - 1; j++)
			{
				for (int k = 0; k < GRID_WIDTH; k++)
				{
					int index = k + j * GRID_WIDTH;
					gameGrid[index] = gameGrid[index + GRID_WIDTH];
					gameGrid[index + GRID_WIDTH] = false;
					cubes[index]->material = cubes[index + GRID_WIDTH]->material;
				}
			}
		}
	}

	// Reward points for cleared lines
	if (cleared > 0) 
	{
		score += scores[cleared - 1];
	}
}

// Retrieves the position of the ghost block vertically in the format x=index, y=world
XMFLOAT2 BlockManager::getGhostPos() {
	int tx = targetX;
	int ty = targetY;
	
	while (canOccupy(tx, --ty));
	ty++;

	return XMFLOAT2(ty, ty * blockWidth + min.y);
}

// Copies the elements from the source array to the target array
void BlockManager::copy(bool* src, bool* dest, int num) {
	for (int i = 0; i < num; i++) {
		dest[i] = src[i];
	}
}

// Shuffles the order of block types to spawn
void BlockManager::shuffle() {
	int held = -1;
	if (heldId >= 0) {
		held = typeOrder[heldId];
	}
	for (int i = 0; i < numBlocks; i++) {
		typeOrder[i] = -1;
	}
	for (int i = 0; i < numBlocks; i++) {
		int index;
		do 
		{
			index = rand() % numBlocks;
		} 
		while (typeOrder[index] != -1);
		typeOrder[index] = i;
		if (i == held) {
			heldId = index;
		}
	}
}
