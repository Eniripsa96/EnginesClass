#ifndef MESHESMATERIALS_H
#define MESHESMATERIALS_H

#include <map>

#include "ObjLoader.h"
#include "Mesh.h"
#include "ParticleMesh.h"
#include "Material.h"
#include "Shaders.h"

class MeshesMaterials
{
public:
	MeshesMaterials();
	~MeshesMaterials();

	static void Destructor(void);
	static void LoadMeshesAndMaterials(ID3D11Device*, ID3D11DeviceContext*);

	static map<char*, Mesh*> meshes;
	static map<char*, Material*> materials;
};

#endif