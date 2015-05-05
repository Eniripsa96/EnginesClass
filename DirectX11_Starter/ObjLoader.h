#ifndef OBJLOADER_H
#define OBJLOADER_H

//#include "GameManager.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "Mesh.h"

using namespace DirectX;
using namespace std;

class ObjLoader
{
public:
	ObjLoader();
	~ObjLoader();

	UINT Load(char* fileName, ID3D11Device* device, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer);
};

#endif