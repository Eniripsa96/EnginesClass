#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <DirectXMath.h>
#include <vector>
#include <SpriteFont.h>
#include <SpriteBatch.h>

#include "DirectXGame.h"
#include "GameObject.h"
#include "Button.h"
#include "Camera.h"
#include "BlockManager.h"
#include "ObjLoader.h"
#include "InputLayouts.h"
#include "ParticleSystem.h"

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// For DirectX Math
using namespace DirectX;
using namespace std;

enum GAME_STATE
{
	MENU,
	GAME,
	GAME_OVER,
	DEBUG
};

struct BLEND_DESC : public D3D11_BLEND_DESC {};

// Demo class which extends the base DirectXGame class
class GameManager : public DirectXGame
{
public:
	GameManager(HINSTANCE hInstance);
	~GameManager();

	// Overrides for base level methods
	bool Init();
	void CreateSamplers();
	void LoadShadersAndInputLayout();
	void LoadPixelShader(wchar_t* file, ID3D11PixelShader** shader);
	void LoadVertexShader(wchar_t* file, LAYOUT inputLayoutType, ID3D11VertexShader** shader);
	void LoadGeometryShader(wchar_t* file, ID3D11GeometryShader** shader);
	void BuildBlockTypes();
	void LoadMeshesAndMaterials();
	void CreateShadowMapResources();
	void OnResize();
	void UpdateScene(float dt);
	void CheckKeyBoard(float dt);
	void DrawScene();	// Deprecated

	// For handing mouse input
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

	// Shaders
	ID3D11PixelShader** pixelShaders;
	ID3D11PixelShader* pixelShader;
	ID3D11PixelShader* grayscaleShader;
	ID3D11PixelShader* sepiaShader;
	ID3D11PixelShader* inverseShader;
	ID3D11VertexShader* vertexShader;
	ID3D11VertexShader* particleVertexShader;
	ID3D11GeometryShader* particleGeometryShader;
	ID3D11PixelShader* particlePixelShader;
	UINT activeShader;
	UINT shaderCount = 4;

	// A few more odds and ends we'll need
	ID3D11InputLayout* inputLayout;
	ID3D11InputLayout* particleInputLayout;

	// Blend state
	ID3D11BlendState* blendState;

	// Constant buffer info
	ID3D11Buffer* vsConstantBuffer;
	ID3D11Buffer* gsConstantBuffer;;
	VertexShaderConstantBufferLayout dataToSendToVSConstantBuffer;
	GeometryShaderConstantBufferLayout dataToSendToGSConstantBuffer;

	GAME_STATE gameState;

private:
	std::vector<GameObject*> gameObjects;
	std::vector<UIObject*> gameUIObjects;
	std::vector<UIObject*> menuObjects;
	std::vector<UIObject*> gameOverObjects;
	BlockManager* blockManager;

	SpriteBatch* spriteBatch;
	SpriteFont* spriteFont24;
	SpriteFont* spriteFont32;
	SpriteFont* spriteFont72;

	Mesh* triangleMesh;
	Mesh* quadMesh;
	Mesh* cubeMesh;
	Mesh* jBlockMesh;
	Mesh* lBlockMesh;
	Mesh* leftBlockMesh;
	Mesh* longBlockMesh;
	Mesh* rightBlockMesh;
	Mesh* squareBlockMesh;
	Mesh* stairsBlockMesh;
	Mesh* frameMesh;
	Mesh* environmentMesh;
	Mesh* particleMesh;

	Material* shapeMaterial;
	Material* buttonMaterial;
	Material* titleMaterial;
	Material* labelMaterial;
	Material* jBlockMaterial;
	Material* lBlockMaterial;
	Material* leftBlockMaterial;
	Material* longBlockMaterial;
	Material* rightBlockMaterial;
	Material* squareBlockMaterial;
	Material* stairsBlockMaterial;
	Material* frameMaterial;
	Material* tileMaterial;
	Material* particleMaterial;

	ID3D11SamplerState* linearSampler;
	ID3D11SamplerState* pointSampler;
	ID3D11SamplerState* anisotropicSampler;

	Block* blocks;
	vector<GameObject*> cubes;
	bool canRotate = false;

	Button* playButton;
	Button* quitButton;
	Button* mainMenuButton;
	UIObject* scoreLabel;

	ParticleSystem* particleSystem;
	Camera* camera;
	XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	const float CAMERA_MOVE_FACTOR = 10.0f;
	const float CAMERA_TURN_FACTOR = 1.0f;

	XMFLOAT4X4 shadowView;
	XMFLOAT4X4 shadowProjection;
	ID3D11Texture2D* shadowTex;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11VertexShader* shadowVS;
	ID3D11PixelShader* shadowPS;
	ID3D11InputLayout* shadowIL;
};

#endif