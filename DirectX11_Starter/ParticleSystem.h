#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <utility>

#include "Camera.h"
#include "MeshesMaterials.h"

using namespace DirectX;

struct PartParams
{
	XMFLOAT3 position;
	XMFLOAT3 color;
	float lifeTime;
	int numParts;
};

class ParticleSystem
{
public:
	// pos, color, age/fadeout, spread, number, shapes?
	ParticleSystem(GeometryShaderConstantBufferLayout*, XMFLOAT3*, XMFLOAT3*, float, int);
	~ParticleSystem();

	// Time elapsed since the systm was reset
	float GetAge() const;
	Material* GetMaterial() const;

	void Reset();
	void Draw(ID3D11DeviceContext*, const Camera&, ID3D11Buffer*, GeometryShaderConstantBufferLayout*);
	void Update(float);

private:
	void BuildVB(ID3D11Device*);

	ParticleSystem(const ParticleSystem&);
	ParticleSystem& operator=(const ParticleSystem&);

private:
	ParticleMesh* mesh;
	Material* material;

	PartParams params;

	ID3D11ShaderResourceView* oneD_SRV;
	GeometryShaderConstantBufferLayout* cBufferData;

	XMFLOAT4X4 world;
	XMFLOAT4 spawnPos;
	int numParticles;
	
	float age;
	float MAX_AGE;
};

#endif