#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(XMFLOAT3* pos, XMFLOAT3* color, float lifeTime, int numP)
{
	// Start out without activating the particle effect
	age = 0.0f;
	
	spawnPos = XMFLOAT4(pos->x, pos->y, pos->z, 1.0f);

	// Set the material and mesh (which we initialize here to give it 
	material = MeshesMaterials::materials["particle"];
	MeshesMaterials::meshes["particle"] = new ParticleMesh(material->device, material->deviceContext, pos);	// CHANGE THIS TO WORK FOR MULTIPLE PARTICLE EFFECTS
	mesh = (ParticleMesh*)MeshesMaterials::meshes["particle"];

	// Initialize the world matrix
	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));

	// Create the 1D randomized textures for GPU randomness
	oneD_SRV = Material::CreateRandomTex(mesh->device);

	// NOTE:
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

	// Reset age and position (world matrix)
	age = INITIAL_AGE;
	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));
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

	// TODO: Should only have to set this once, not every update
	cBufferData->spawnPos = spawnPos;

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