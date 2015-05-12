#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(GeometryShaderConstantBufferLayout* cBuff, XMFLOAT3* pos, XMFLOAT3* color, float lifeTime, int numP)
{
	// Start out without activating the particle effect
	age = 0.0f;
	MAX_AGE = lifeTime;
	
	// Cap the number of particles at 75 (GS max supported)
	numP = (numP > 75) ? 75 : numP;
	
	// Initialize the world matrix
	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));

	// Set data from params
	spawnPos = XMFLOAT4(pos->x, pos->y, pos->z, 1.0f);
	numParticles = numP;
	cBufferData = cBuff;
	cBufferData->spawnPos = spawnPos;
	cBufferData->misc.z = (float)numParticles;
	cBufferData->world = world;

	// Set the material and mesh (which we initialize here to give it 
	material = MeshesMaterials::materials["particle"];
	MeshesMaterials::meshes["particle"] = new ParticleMesh(material->device, material->deviceContext, pos, color);	// CHANGE THIS TO WORK FOR MULTIPLE PARTICLE EFFECTS
	mesh = (ParticleMesh*)MeshesMaterials::meshes["particle"];

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

void ParticleSystem::Reset(XMFLOAT3* pos, XMFLOAT3* col, float lifeTime, int numP)
{
	age = 0.0f;
	MAX_AGE = lifeTime;

	// Cap the number of particles at 75 (GS max supported)
	numP = (numP > 75) ? 75 : numP;

	// Set data from params
	spawnPos = XMFLOAT4(pos->x, pos->y, pos->z, 1.0f);
	numParticles = numP;
	
	cBufferData->spawnPos = spawnPos;
	cBufferData->misc.z = (float)numParticles;
	cBufferData->world = world;

	// Refresh particle mesh with new params
	delete MeshesMaterials::meshes["particle"];
	MeshesMaterials::meshes["particle"] = new ParticleMesh(material->device, material->deviceContext, pos, col);	// CHANGE THIS TO WORK FOR MULTIPLE PARTICLE EFFECTS
	mesh = (ParticleMesh*)MeshesMaterials::meshes["particle"];
}

// Restart the particle system
void ParticleSystem::Emit()
{
	mesh->firstTime = true;

	// Reset age and position (world matrix)
	age = MAX_AGE;
	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));
	cBufferData->world = world;
}

Material* ParticleSystem::GetMaterial() const
{
	return this->material;
}

void ParticleSystem::Update(float dt)
{
	// Grab the current view-projection
	//XMMATRIX VP = XMMatrixMultiply(XMLoadFloat4x4(&cam.viewMatrix), XMLoadFloat4x4(&cam.projectionMatrix));

	// Update age
	age -= 1.0f * dt;

	// Update const buffer with current age and delta time
	cBufferData->misc.x = age;
	cBufferData->misc.y = dt;
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