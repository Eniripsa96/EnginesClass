#include "MeshesMaterials.h"

Mesh* MeshesMaterials::triangleMesh = NULL;
Mesh* MeshesMaterials::quadMesh = NULL;
Mesh* MeshesMaterials::cubeMesh = NULL;
Mesh* MeshesMaterials::frameMesh = NULL;
Mesh* MeshesMaterials::environmentMesh = NULL;
Mesh* MeshesMaterials::particleMesh = NULL;

Material* MeshesMaterials::shapeMaterial = NULL;
Material* MeshesMaterials::buttonMaterial = NULL;
Material* MeshesMaterials::titleMaterial = NULL;
Material* MeshesMaterials::labelMaterial = NULL;
Material* MeshesMaterials::frameMaterial = NULL;
Material* MeshesMaterials::tileMaterial = NULL;
Material* MeshesMaterials::particleMaterial = NULL;

MeshesMaterials::MeshesMaterials()
{
}

MeshesMaterials::~MeshesMaterials()
{
}

void MeshesMaterials::Destructor()
{
	// Clean up meshes
	delete triangleMesh;
	delete quadMesh;
	delete cubeMesh;
	delete frameMesh;
	delete environmentMesh;
	delete particleMesh;

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
	cubeMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("frame.txt", device, &vertexBuffer, &indexBuffer);
	frameMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("environment.txt", device, &vertexBuffer, &indexBuffer);
	environmentMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);

	// Create 2D meshes
	triangleMesh = new Mesh(device, deviceContext, TRIANGLE);
	quadMesh = new Mesh(device, deviceContext, QUAD);
	particleMesh = new Mesh(device, deviceContext, PARTICLE);
}
