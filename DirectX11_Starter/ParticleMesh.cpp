#include "ParticleMesh.h"
#include "Shaders.h"

ParticleMesh::ParticleMesh(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT3* pos, XMFLOAT3* col) : Mesh(device, context, PARTICLE)
{
	// Next time this draws will be the first time, so EMIT
	firstTime = true;

	position = *pos;
	color = *col;

	CreateParticlePoints();
}


ParticleMesh::~ParticleMesh()
{
	// Release the D3D11 buffer
	ReleaseMacro(streamOutVB);

	// NOTE: Implicitly calls base destructor
}

void ParticleMesh::CreateParticlePoints()
{
	srand(time(NULL));

	vector<Particle> particles;
	
	// Just make one particle here, the emitter
	for (int i = 0; i < 1; i++)
	{
		// NOTE: This gives the semi-even distribution of velocities directly outwards
		XMFLOAT3 vel = XMFLOAT3((rand() % 200) / 100.0f - 0.5f, (rand() % 200) / 100.0f - 0.5f, 0.0f);
		particles.push_back(Particle{ position, vel, XMFLOAT2(PARTICLE_SIZE, PARTICLE_SIZE), color });
	}

	CreateGeometryBuffers(&particles[0]);
}

void ParticleMesh::CreateGeometryBuffers(Particle particles[])
{
	// Create a description for a stream-out capable vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(Particle) * (int)PARTICLE; // Number of vertices in the "model" you want to draw
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Set up the vertex data and create buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = particles;
	HR(device->CreateBuffer(&vbd, &initialVertexData, &drawVB));

	// Create stream out vertex buffer (without any data)
	HR(device->CreateBuffer(&vbd, NULL, &streamOutVB));

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;

	// Set up the indices and create index buffer
	vector<UINT> particleIndices;
	for (UINT i = 0; i < (UINT)PARTICLE; i++)
		particleIndices.push_back(i);

	initialIndexData.pSysMem = &particleIndices[0];
	ibd.ByteWidth = sizeof(UINT) * (int)PARTICLE; // Number of indices in the "model" you want to draw
	HR(device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer));
}

void ParticleMesh::Draw()
{
	// Set buffers in the input assembler
	UINT stride = sizeof(Particle);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &drawVB, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set our stream VB as the SO target
	deviceContext->SOSetTargets(1, &streamOutVB, &offset);

	if (firstTime)
	{
		deviceContext->GSSetShader(Shaders::emitterGS, NULL, 0);
		deviceContext->GSSetSamplers(0, 1, &Samplers::linearSampler);
		firstTime = false;
	}
	else
	{
		// Hook up the proper shaders for this step
		deviceContext->GSSetShader(Shaders::streamOutGeometryShader, NULL, 0);
	}
	deviceContext->PSSetShader(NULL, NULL, 0);	// Don't need a PS for either GS stage

	// Draw the current vertex list using stream-out only to update them.
	// The updated vertices are streamed-out to the target VB
	deviceContext->DrawIndexed(
		(int)PARTICLE,	// The number of indices we're using in this draw
		0,
		0);

	// Done streaming-out --> unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	deviceContext->SOSetTargets(1, bufferArray, &offset);

	// Copy new data into the VB we are drawing with
	std::swap(drawVB, streamOutVB);

	// Hook up the proper shaders for this step
	deviceContext->GSSetShader(Shaders::particleGeometryShader, NULL, 0);
	deviceContext->PSSetShader(Shaders::particlePixelShader, NULL, 0);

	// Bind our VB to the IA for drawing
	deviceContext->IASetVertexBuffers(0, 1, &(drawVB), &stride, &offset);

	// Draw the updated mesh
	deviceContext->DrawIndexed(
		(int)PARTICLE,	// The number of indices we're using in this draw
		0,
		0);
}