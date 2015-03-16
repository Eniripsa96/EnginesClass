#ifndef MESHESMATERIALS_H
#define MESHESMATERIALS_H

#include "Mesh.h"
#include "ObjLoader.h"
#include "Material.h"

#include <map>

class MeshesMaterials
{
public:
	MeshesMaterials();
	~MeshesMaterials();

	static void Destructor(void);
	static void LoadMeshesAndMaterials(ID3D11Device*, ID3D11DeviceContext*);
	 
	static Material* shapeMaterial;
	static Material* buttonMaterial;
	static Material* titleMaterial;
	static Material* labelMaterial;
	static Material* frameMaterial;
	static Material* tileMaterial;
	static Material* particleMaterial;

	static map<char*, Mesh*> meshes;
	static map<char*, Material*> materials;
};

#endif