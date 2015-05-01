#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(ParticleMesh* mesh, Material* mat)
{
	this->mesh = mesh;
	this->material = mat;

	age = 0.0f;

	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));

	oneD_SRV = Material::CreateRandomTex(mesh->device);

	// Need two Geometry buffers, an update one and a drawing one
	// Bind SO stuff (bind vb to SO), then unbind PS so that GS goes straight to SO and no further
	// Draw with VS, GS->SO
	// Set VB regularly
	// Draw with VS, PS, GS
}

ParticleSystem::~ParticleSystem()
{
	ReleaseMacro(oneD_SRV);
}

// Restart the particle system
void ParticleSystem::Reset()
{
	mesh->firstTime = true;

	age = INITIAL_AGE;
	velocity = INITIAL_VEL;
	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));

	//ReleaseMacro(oneD_SRV);
	//oneD_SRV = Material::CreateRandomTex(mesh->device);

	// TODO Need to figure out random
	// Need to modify movement on GPU by 'dt'
}

Material* ParticleSystem::GetMaterial() const
{
	return this->material;
}

void ParticleSystem::Update(GeometryShaderConstantBufferLayout* cBufferData, float dt)
{
	// Grab the current view-projection
	//XMMATRIX VP = XMMatrixMultiply(XMLoadFloat4x4(&cam.viewMatrix), XMLoadFloat4x4(&cam.projectionMatrix));

	age -= 1.0f * dt;
	cBufferData->age = XMFLOAT4(age, dt, 0.0f, 0.0f);

	XMMATRIX tempWorld = XMMatrixTranslation(0.0f, 0.0f * dt, 0.0f );
	XMStoreFloat4x4(&world, XMLoadFloat4x4(&world) * tempWorld);
	cBufferData->world = world;
}

void ParticleSystem::Draw(ID3D11DeviceContext* dc, const Camera& cam, ID3D11Buffer* cBuffer, GeometryShaderConstantBufferLayout* cBufferData)
{
	if (age > 0.0f)
	{
		// Update constant buffer with camera info
		cBufferData->camPos = cam.GetPos();

		// [UPDATE] Update the constant buffer itself
		dc->UpdateSubresource(
			cBuffer,
			0,
			NULL,
			cBufferData,
			0,
			0
			);

		// [DRAW] Set the constant buffer in the device
		dc->GSSetConstantBuffers(
			0,
			1,
			&(cBuffer)
			);

		mesh->deviceContext->GSSetShaderResources(0, 1, &oneD_SRV);

		// Draw mesh and material
		material->Draw();
		mesh->Draw();
	}
}