#include "MeshesMaterials.h"

map<char*, Mesh*> MeshesMaterials::meshes = {};
map<char*, Material*> MeshesMaterials::materials = {};

MeshesMaterials::MeshesMaterials()
{
}

MeshesMaterials::~MeshesMaterials()
{
}

void MeshesMaterials::Destructor()
{
	// Clean up meshes
	std::map<char*, Mesh*>::iterator it1 = meshes.begin();
	for (; it1 != meshes.end(); ++it1)
	{
		delete it1->second;
	}

	// Clean up materials
	std::map<char*, Material*>::iterator it2 = materials.begin();
	for (; it2 != materials.end(); ++it2)
	{
		delete it2->second;
	}
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
	materials.insert(pair<char*, Material*>("shape", new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"image.png")));
	materials.insert(pair<char*, Material*>("button", new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"button.png")));
	materials.insert(pair<char*, Material*>("title", new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"title.png")));
	materials.insert(pair<char*, Material*>("label", new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"label.png")));
	materials.insert(pair<char*, Material*>("frame", new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::linearSampler, L"texFrame.png")));
	materials.insert(pair<char*, Material*>("tile", new Material(device, deviceContext, Shaders::vertexShader, Shaders::pixelShader, Samplers::anisotropicSampler, L"tile.png")));
	materials.insert(pair<char*, Material*>("particle", new Material(device, deviceContext, Shaders::particleVertexShader, Shaders::particlePixelShader, Samplers::linearSampler, L"texLBlock.png", Shaders::particleGeometryShader)));

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
	meshes.insert(pair<char*, Mesh*>("particle", new ParticleMesh(device, deviceContext)));
}
