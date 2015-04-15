#ifndef PARTICLEMESH_H
#define PARTICLEMESH_H

#include "Mesh.h"
class ParticleMesh :
	public Mesh
{
public:
	ParticleMesh(ID3D11Device*, ID3D11DeviceContext*);

	~ParticleMesh();

	void CreateParticlePoints();
	void CreateGeometryBuffers(Particle[]);
	void Draw();

	ID3D11Buffer* streamOutVB;

private:
	const float PARTICLE_SIZE = 0.1875f;
};

#endif