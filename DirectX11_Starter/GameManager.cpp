// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the compiled shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#include "GameManager.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include <vector>
#include "Math.h"

// Background color
const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Make the game, initialize and run
	GameManager game(hInstance);

	if (!game.Init())
		return 0;

	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

GameManager::GameManager(HINSTANCE hInstance) : DirectXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Demo DX11 Game";
	windowWidth = 800;
	windowHeight = 600;

	gameState = MENU;

#pragma region network test code

	//bool worked = network.tryHost();
	//network.startServer();

	/*bool worked = network.tryConnect();
	network.startListening();*/

#pragma endregion

#pragma region SSE test code
	/*
	float c1 = cos(1.0f);
	float s1 = sin(1.0f);
	float c2 = cos(2.0f);
	float s2 = sin(2.0f);

	SSEQuaternion::Initialize();

	NormalQuaternion q1(s1 * 0.5f, 0.0f, s1 * sqrt(3.0f) / 2.0f, c1);
	NormalQuaternion q2(0.0f, s2 * sqrt(2.0f) / 2.0f, s2 * sqrt(2.0f) / 2.0f, c2);
	NormalQuaternion q3;
	std::clock_t start = std::clock();
	for (int i = 0; i < 400000; i++)
	{
		q1.slerp(q2, q3, 0.5f);
	}
	double duration1 = std::clock() - start;

	SSEQuaternion q4(s1 * 0.5f, 0.0f, s1 * sqrt(3.0f) / 2.0f, c1);
	SSEQuaternion q5(0.0f, s2 * sqrt(2.0f) / 2.0f, s2 * sqrt(2.0f) / 2.0f, c2);
	SSEQuaternion q6;;

	// SSE Attempt 1 - Two unloads, no trig
	start = std::clock();
	for (int i = 0; i < 400000; i++)
	{
		q4.slerp(q5, q6, 0.5f);
	}
	double duration2 = std::clock() - start;

	// SSE Attempt 2 - One unload, regular acos call
	start = std::clock();
	for (int i = 0; i < 400000; i++)
	{
		q4.slerp2(q5, q6, 0.5f);
	}
	double duration3 = std::clock() - start;

	// SSE Attempt 3 - All SSE functions
	start = std::clock();
	for (int i = 0; i < 400000; i++)
	{
		q4.slerp3(q5, q6, 0.5f);
	}
	double duration4 = std::clock() - start;

	// SSE Attempt 5 - One unload, regular acos call, parallel sin calculations
	start = std::clock();
	for (int i = 0; i < 400000; i++)
	{
		q4.slerp5(q5, q6, 0.5f);
	}
	double duration6 = std::clock() - start;

	int i = 0;
	*/
#pragma endregion
}

// Clean up here
GameManager::~GameManager()
{
	// Clean up objects
	for (UINT i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	for (UINT i = 0; i < menuObjects.size(); i++)
	{
		delete menuObjects[i];
	}
	for (UINT i = 0; i < gameUIObjects.size(); i++)
	{
		delete gameUIObjects[i];
	}
	for (UINT i = 0; i < gameOverObjects.size(); i++)
	{
		delete gameOverObjects[i];
	}
	gameObjects.clear();
	menuObjects.clear();
	gameOverObjects.clear();
	gameUIObjects.clear();

	// Clean up engine components
	MeshesMaterials::Destructor();
	Shaders::Destructor();
	delete camera;
	delete particleSystem;
	
	delete spriteBatch;
	delete spriteFont24;
	delete spriteFont32;
	delete spriteFont72;

	// Release DirectX variables
	ReleaseMacro(blendState);

	ReleaseMacro(shadowTex);
	ReleaseMacro(shadowSRV);
	ReleaseMacro(shadowDSV);
	ReleaseMacro(shadowIL);

	ReleaseMacro(Samplers::linearSampler);
	ReleaseMacro(Samplers::anisotropicSampler);
	ReleaseMacro(Samplers::pointSampler);

	ReleaseMacro(InputLayouts::Vertex);
	ReleaseMacro(InputLayouts::Particle);
	ReleaseMacro(InputLayouts::Shadow);
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool GameManager::Init()
{
	if (!DirectXGame::Init())
		return false;

	// Initialize and load core engine components
	Samplers::CreateSamplers(device, deviceContext);
	Shaders::LoadShadersAndInputLayout(device, deviceContext);
	MeshesMaterials::LoadMeshesAndMaterials(device, deviceContext);
	camera = new Camera();
	particleSystem = new ParticleSystem((ParticleMesh*)MeshesMaterials::meshes["particle"], MeshesMaterials::materials["particle"]);

	// Initialize the shadow camera
		// None

	// Load the fonts for the games
	spriteBatch = new SpriteBatch(deviceContext);
	spriteFont24 = new SpriteFont(device, L"jing24.spritefont");
	spriteFont32 = new SpriteFont(device, L"jing32.spritefont");
	spriteFont72 = new SpriteFont(device, L"jing72.spritefont");

	// Load the frame
	gameObjects.emplace_back(new GameObject(MeshesMaterials::meshes["frame"], MeshesMaterials::materials["frame"], &XMFLOAT3(-3.0f, -5.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));
	gameObjects.emplace_back(new GameObject(MeshesMaterials::meshes["environment"], MeshesMaterials::materials["tile"], &XMFLOAT3(-50.0f, -5.0f, -75.0f), &XMFLOAT3(0, 0, 0)));
	gameObjects.emplace_back(new GameObject(MeshesMaterials::meshes["cube"], MeshesMaterials::materials["shape"], &XMFLOAT3(0.0f, 0.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));

	// Create buttons for UI
	playButton = new Button(MeshesMaterials::meshes["quad"], MeshesMaterials::materials["button"], &XMFLOAT3(200, 250, 0), spriteBatch, spriteFont32, L"Play");
	quitButton = new Button(MeshesMaterials::meshes["quad"], MeshesMaterials::materials["button"], &XMFLOAT3(200, 400, 0), spriteBatch, spriteFont32, L"Quit");
	//mainMenuButton = new Button(MeshesMaterials::meshes["quad"], MeshesMaterials::materials["button"], &XMFLOAT3(200, 300, 0), spriteBatch, spriteFont32, L"Main Menu");
	menuObjects.emplace_back(new UIObject(MeshesMaterials::meshes["quad"], MeshesMaterials::materials["title"], &XMFLOAT3(100, 50, 0), spriteBatch, spriteFont72, L"Tetris"));
	menuObjects.emplace_back(playButton);
	menuObjects.emplace_back(quitButton);

	// Blend state - enabling alpha blending
	BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &blendState);

	// Set up the world matrix for each mesh
	XMMATRIX W = XMMatrixIdentity();
	for (UINT i = 0; i < gameObjects.size(); i++)
		XMStoreFloat4x4(&(gameObjects[i]->worldMatrix), XMMatrixTranspose(W));

	return true;
}

void GameManager::CreateShadowMapResources() 
{
	ReleaseMacro(shadowTex);
	ReleaseMacro(shadowDSV);
	ReleaseMacro(shadowSRV);

	// Texture
	D3D11_TEXTURE2D_DESC texDesc;
	//texDesc.Width = 2048;
	//texDesc.Height = 2048;
	texDesc.Width = windowWidth;
	texDesc.Height = windowHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &shadowTex));

	// Depth Stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(shadowTex, &descDSV, &shadowDSV));

	// Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR(device->CreateShaderResourceView(shadowTex, &srvDesc, &shadowSRV));
}

#pragma endregion

#pragma region Game Loop

// Updates the buffers (and pushes them to the buffer on the device)
// and draws for each gameObject
void GameManager::UpdateScene(float dt)
{
	CheckKeyBoard(dt);

	// Update the game
		// None

	// Set active mesh list
	std::vector<GameObject*> *meshObjects = 0;
	if (gameState == GAME || gameState == DEBUG)
	{
		gameObjects[gameObjects.size() - 1]->Rotate(&XMFLOAT3(0.0f * dt, 1.0f * dt, 0.0f * dt));

		meshObjects = &gameObjects;
	}
	
	// Active UI list
	std::vector<UIObject*> *uiObjects = 0;
	if (gameState == MENU) uiObjects = &menuObjects;
	if (gameState == GAME || gameState == DEBUG) uiObjects = &gameUIObjects;
	if (gameState == GAME_OVER) uiObjects = &gameOverObjects;

	// [DRAW] Set up the input assembler for objects
	deviceContext->IASetInputLayout(InputLayouts::Vertex);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Update each mesh
	if (meshObjects) {
		for (UINT i = 0; i < meshObjects->size(); i++) {
			if (gameState != DEBUG) (*meshObjects)[i]->Update(dt);
		}
	}

	// --------- Shadow Map Generation ------------------------------------------------------------------
	
	// Update shadow camera
	//shadowCam->Update(dt);

	// [UPDATE] Update constant buffer data
	Shaders::dataToSendToVSConstantBuffer.view = camera->viewMatrix;
	Shaders::dataToSendToVSConstantBuffer.projection = projectionMatrix;
	Shaders::dataToSendToVSConstantBuffer.lightView = shadowView;
	Shaders::dataToSendToVSConstantBuffer.lightProjection = shadowProjection;
	Shaders::dataToSendToVSConstantBuffer.lightDirection = XMFLOAT4(2.0f, -3.0f, 1.0f, 0.95f);
	Shaders::dataToSendToVSConstantBuffer.color = XMFLOAT4(1, 1, 1, 1);

	Shaders::dataToSendToGSConstantBuffer.view = camera->viewMatrix;
	Shaders::dataToSendToGSConstantBuffer.projection = projectionMatrix;

	// Shadow map
	deviceContext->OMSetRenderTargets(0, 0, shadowDSV);
	deviceContext->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deviceContext->VSSetShader(Shaders::shadowVS, 0, 0);
	deviceContext->PSSetShader(0, 0, 0);

	// Draw mesh objects that can cast shadows
	if (meshObjects) {
		for (UINT i = 0; i < meshObjects->size(); i++) {
			(*meshObjects)[i]->Draw(deviceContext, Shaders::vsConstantBuffer, &Shaders::dataToSendToVSConstantBuffer);
		}
	}

	// ----------- Normal Rendering --------------------------------------------------------

	// Update the camera
	camera->Update(dt);

	// Clear the buffer
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Bind shadow map
	deviceContext->PSSetShaderResources(1, 1, &shadowSRV);
	deviceContext->PSSetSamplers(1, 1, &(Samplers::pointSampler));

	// Set the shaders
	Shaders::Update();

	// Bind shadow map texture
	deviceContext->PSSetShaderResources(1, 1, &shadowSRV);
	deviceContext->PSSetSamplers(1, 1, &(Samplers::pointSampler));
	
	// Draw each mesh
	if (meshObjects)
	{
		for (UINT i = 0; i < meshObjects->size(); i++)
		{
			(*meshObjects)[i]->Draw(deviceContext, Shaders::vsConstantBuffer, &Shaders::dataToSendToVSConstantBuffer);
		}
	}

	// Draw the particle system	
	if (gameState == GAME || gameState == DEBUG)
	{
		// [DRAW] Set up the input assembler for particle system
		deviceContext->IASetInputLayout(InputLayouts::Particle);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		particleSystem->GetMaterial()->SetShaders();

		if (gameState != DEBUG)
			// [UPDATE] Update the particle system 
			particleSystem->Update(&Shaders::dataToSendToGSConstantBuffer, dt);

		// [DRAW] Draw the particle system
		particleSystem->Draw(deviceContext, *camera, Shaders::gsConstantBuffer, &Shaders::dataToSendToGSConstantBuffer);

		deviceContext->GSSetShader(NULL, 0, 0);
	}

	
	// Draw UI Elements
	if (uiObjects)
	{
		spriteBatch->Begin();
		for (UINT i = 0; i < uiObjects->size(); i++)
		{
			// [DRAW] Draw the object
			(*uiObjects)[i]->Draw(deviceContext, Shaders::vsConstantBuffer, &Shaders::dataToSendToVSConstantBuffer);
		}
		spriteBatch->End();

		deviceContext->OMSetBlendState(blendState, NULL, 0xffffffff);
		deviceContext->OMSetDepthStencilState(0, 0);
	}
	
	ID3D11ShaderResourceView* nullView = NULL;
	deviceContext->PSSetShaderResources(1, 1, &nullView);

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

// NOTE: DEPRECATED
// Clear the screen, redraw everything, present
void GameManager::DrawScene() { } 

#pragma endregion

#pragma region User Input

struct test : packetStruct {
	int num1 : 4;
	int num2 : 4;
	int num3 : 8;
	int num4 : 16;
};

// Continuous while key pressed
bool first = true;
bool holding = false;
void GameManager::CheckKeyBoard(float dt)
{
	// Game controls
		// Currently none
	
	// Camera controls

	// Move camera (WASD)
	if (GetAsyncKeyState('A'))
		camera->MoveHorizontal(-CAMERA_MOVE_FACTOR * dt);
	else if (GetAsyncKeyState('D'))
		camera->MoveHorizontal(CAMERA_MOVE_FACTOR * dt);
	if (GetAsyncKeyState('W'))
		camera->MoveDepth(CAMERA_MOVE_FACTOR * dt);
	else if (GetAsyncKeyState('S'))
		camera->MoveDepth(-CAMERA_MOVE_FACTOR * dt);

	// Space bar to activate particle effect
	if (GetAsyncKeyState(' '))
		particleSystem->Reset();

	// Change height of camera (QE)
	if (GetAsyncKeyState('Q'))
		camera->MoveVertical(CAMERA_MOVE_FACTOR * dt);
	else if (GetAsyncKeyState('E'))
		camera->MoveVertical(-CAMERA_MOVE_FACTOR * dt);

	// Network testing
	if (!holding) {
		if (first) {
			if (GetAsyncKeyState('Z')) {
				network.startServer();
				holding = true;
			}
			else if (GetAsyncKeyState('X')) {
				network.startListening();
				holding = true;
			}
		}
		else {
			if (GetAsyncKeyState('C')) {
				test data;
				data.num1 = 1;
				data.num2 = 2;
				data.num3 = 3;
				data.num4 = 4;
				network.emit(&data);
				holding = true;
			}
			else if (GetAsyncKeyState('V')) {
				if (network.hasData())
				{
					packet data = network.getData();

					test result = *((test*)data.buffer);
					int i = 1;
				}
				holding = true;
			}
		}
	}
	else if (!GetAsyncKeyState('Z') && !GetAsyncKeyState('X') && !GetAsyncKeyState('C') && !GetAsyncKeyState('V')) {
		holding = false;
		first = false;
	}
}

// Once per key press
LRESULT GameManager::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		// Change the active shader
		case VK_TAB:
			Shaders::activeShader = (Shaders::activeShader + 1) % Shaders::shaderCount;
			break;
		}
	}

	return DirectXGame::MsgProc(hwnd, msg, wParam, lParam);
}

// These methods don't do much currently, but can be used for mouse-related input

void GameManager::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hMainWnd);
}

void GameManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();

	// Main menu buttons
	if (gameState == MENU)
	{
		if (playButton->IsOver(x, y))
		{
			gameState = GAME;
		}
		if (quitButton->IsOver(x, y))
		{
			PostQuitMessage(0);
		}
	}
}

void GameManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - prevMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - prevMousePos.y));

		camera->RotateY(-dx);
		camera->Pitch(-dy);
	}

	playButton->Update(x, y);
	quitButton->Update(x, y);

	prevMousePos.x = x;
	prevMousePos.y = y;
}


#pragma endregion

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void GameManager::OnResize()
{
	// Handle base-level DX resize stuff
	DirectXGame::OnResize();

	// Shadow map projection
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.5f * 3.1415926535f,
		//1,
		AspectRatio(),
		0.1f,
		100.0f);
	XMStoreFloat4x4(&shadowProjection, XMMatrixTranspose(P));

	// Update our projection matrix since the window size changed
	P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100.0f);

	// TODO fix the fact that there is an if statement needed here
	//if (camera)
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));

	CreateShadowMapResources();
}
#pragma endregion