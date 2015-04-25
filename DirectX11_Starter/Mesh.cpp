#include "Mesh.h"
#include "Shaders.h"
#include <d3dcompiler.h>

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, SHAPE type)
{
	// Grab some helpful data
	this->device = device;
	deviceContext = context;
	shapeType = type;

	// Initialize vertices based on shape type
	if (shapeType == TRIANGLE)
		CreateTrianglePoints();
	else if (shapeType == QUAD)
		CreateQuadPoints();
	
	// TODO else if PARTICLE THROW LOG ERROR
}

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, UINT iBufferSize)
{
	this->device = device;
	deviceContext = context;
	drawVB = pVertexBuffer;
	indexBuffer = pIndexBuffer;
	this->iBufferSize = iBufferSize;
}

Mesh::~Mesh()
{
	// Release the D3D11 buffers
	ReleaseMacro(drawVB);
	ReleaseMacro(indexBuffer);
}

void Mesh::CreateTrianglePoints()
{
	// Set up the vertices for a triangle
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), NORMALS_2D, XMFLOAT2(0.5f, 0.0f) },
		{ XMFLOAT3(-1.0f, -0.5f, +0.0f), NORMALS_2D, XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+1.0f, -0.5f, +0.0f), NORMALS_2D, XMFLOAT2(1.0f, 1.0f) },
	};

	CreateGeometryBuffers(vertices);
}

// Create the points for a quad. Particle=true means this quad is for a particle system
void Mesh::CreateQuadPoints()
{
	// Set up the vertices for a quad
	Vertex vertices[] =
	{
		// NOTE TEXTURE COORDS ARE 0,0 TOP LEFT AND 1,1 BOTTOM RIGHT
		{ XMFLOAT3(0, 1, 0), NORMALS_2D, XMFLOAT2(0.0f, 0.0f) },	// Top left
		{ XMFLOAT3(0, 0, 0), NORMALS_2D, XMFLOAT2(0.0f, 1.0f) },	// Bottom left
		{ XMFLOAT3(1, 1, 0), NORMALS_2D, XMFLOAT2(1.0f, 0.0f) },	// Top right
		{ XMFLOAT3(1, 0, 0), NORMALS_2D, XMFLOAT2(1.0f, 1.0f) },	// Bottom right
	};

	CreateGeometryBuffers(vertices);
}

// Creates the vertex and index buffers for a single triangle. Dynamic=true means a dynamic vertex buffer
void Mesh::CreateGeometryBuffers(Vertex vertices[])
{
	// Create description for vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(Vertex) * 3 * (int)shapeType; // Number of vertices in the "model" you want to draw
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Set up the vertex data and create buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &initialVertexData, &drawVB));

	// Create description for the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;

	// Set up the indices and create index buffer
	UINT indices[] = { 0, 2, 1, 1, 2, 3 };
	initialIndexData.pSysMem = indices;
	ibd.ByteWidth = sizeof(UINT) * 3 * (int)shapeType; // Number of indices in the "model" you want to draw
	HR(device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer));
}

void Mesh::Draw()
{
	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &drawVB, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Draw one of our pre-defined shape types
	if (shapeType != NONE)
	{
		// Finally do the actual drawing
		if (shapeType != PARTICLE)
			deviceContext->DrawIndexed(
				3 * (int)this->shapeType,	// The number of indices we're using in this draw
				0,
				0);
	}
	// Draw mesh built from .obj
	else
	{
		deviceContext->DrawIndexed(
			iBufferSize,	// The number of indices we're using in this draw
			0,
			0);
	}
}