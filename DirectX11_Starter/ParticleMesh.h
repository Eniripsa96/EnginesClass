#ifndef PARTICLEMESH_H
#define PARTICLEMESH_H

#include "Mesh.h"
#include "Samplers.h"

class ParticleMesh :
	public Mesh
{
public:
	ParticleMesh(ID3D11Device*, ID3D11DeviceContext*, XMFLOAT3*, XMFLOAT3*, float);

	~ParticleMesh();

	void CreateParticlePoints();
	void CreateGeometryBuffers(Particle[]);
	void Draw();

	ID3D11Buffer* streamOutVB;

	bool firstTime;

private:
	const float PARTICLE_SIZE = 0.1875f / 4.0f;
	float size;
	XMFLOAT3 position;
	XMFLOAT3 color;
};

#endif