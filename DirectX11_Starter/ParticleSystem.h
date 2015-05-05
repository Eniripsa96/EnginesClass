#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <utility>

#include "Camera.h"
#include "MeshesMaterials.h"

using namespace DirectX;

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	// Time elapsed since the systm was reset
	float GetAge() const;
	Material* GetMaterial() const;

	void Reset();
	void Draw(ID3D11DeviceContext* dc, const Camera& cam, ID3D11Buffer* cBuffer, GeometryShaderConstantBufferLayout* cBufferData);
	void Update(GeometryShaderConstantBufferLayout* cBufferData, float dt);

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

private:
	ParticleMesh* mesh;
	Material* material;

	ID3D11ShaderResourceView* oneD_SRV;

	XMFLOAT4X4 world;
	float velocity;
	float age;

	const float INITIAL_VEL = 0.0f;
	const float GRAVITY = -2.0f;
	const float INITIAL_AGE = 10.0f;
};

#endif