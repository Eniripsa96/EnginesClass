#include "MeshesMaterials.h"

Material* MeshesMaterials::shapeMaterial = NULL;
Material* MeshesMaterials::buttonMaterial = NULL;
Material* MeshesMaterials::titleMaterial = NULL;
Material* MeshesMaterials::labelMaterial = NULL;
Material* MeshesMaterials::frameMaterial = NULL;
Material* MeshesMaterials::tileMaterial = NULL;
Material* MeshesMaterials::particleMaterial = NULL;

map<char*, Mesh*> MeshesMaterials::meshes = {};

MeshesMaterials::MeshesMaterials()
{
}

MeshesMaterials::~MeshesMaterials()
{
}

void MeshesMaterials::Destructor()
{
	// Clean up meshes
	std::map<char*, Mesh*>::iterator it = meshes.begin();
	for (; it != meshes.end(); ++it)
	{
		delete it->second;
	}


	// Clean up materials
	delete shapeMaterial;
	delete buttonMaterial;
	delete titleMaterial;
	delete labelMaterial;
	delete frameMaterial;
	delete tileMaterial;
	delete particleMaterial;
}

// Load each of the game's meshes and materials
void MeshesMaterials::LoadMeshesAndMaterials(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Prepare some variables
	ObjLoader loader = ObjLoader();
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int size;

	// Create materials
	shapeMaterial = new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"image.png");
	buttonMaterial = new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"button.png");
	titleMaterial = new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"title.png");
	labelMaterial = new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"label.png");
	frameMaterial = new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"texFrame.png");
	tileMaterial = new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::anisotropicSampler, L"tile.png");
	particleMaterial = new Material(device, deviceContext, Shaders::particleVertexShader, Shaders::particlePixelShader, Samplers::linearSampler, L"texLBlock.png", Shaders::particleGeometryShader);

	// Load meshes
	size = loader.Load("cube.txt", device, &vertexBuffer, &indexBuffer);
	meshes.insert(pair<char*, Mesh*>("cube", new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size)));
	size = loader.Load("frame.txt", device, &vertexBuffer, &indexBuffer);
	meshes.insert(pair<char*, Mesh*>("frame", new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size)));
	size = loader.Load("environment.txt", device, &vertexBuffer, &indexBuffer);
	meshes.insert(pair<char*, Mesh*>("environment", new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size)));

	// Create 2D meshes
	meshes.insert(pair<char*, Mesh*>("triangle", new Mesh(device, deviceContext, TRIANGLE)));
	meshes.insert(pair<char*, Mesh*>("quad", new Mesh(device, deviceContext, QUAD)));
	meshes.insert(pair<char*, Mesh*>("particle", new Mesh(device, deviceContext, PARTICLE)));
}
