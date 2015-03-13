#include "Shaders.h"

ID3D11PixelShader** Shaders::pixelShaders = NULL;
ID3D11PixelShader* Shaders::pixelShader = NULL;
ID3D11PixelShader* Shaders::grayscaleShader = NULL;
ID3D11PixelShader* Shaders::sepiaShader = NULL;
ID3D11PixelShader* Shaders::inverseShader = NULL;
ID3D11VertexShader* Shaders::vertexShader = NULL;
ID3D11VertexShader* Shaders::particleVertexShader = NULL;
ID3D11GeometryShader* Shaders::particleGeometryShader = NULL;
ID3D11PixelShader* Shaders::particlePixelShader = NULL;
UINT Shaders::activeShader = NULL;
ID3D11VertexShader* Shaders::shadowVS = NULL;
ID3D11PixelShader* Shaders::shadowPS = NULL;
ID3D11Buffer* Shaders::vsConstantBuffer = NULL;
ID3D11Buffer* Shaders::gsConstantBuffer = NULL;
VertexShaderConstantBufferLayout Shaders::dataToSendToVSConstantBuffer = {};
GeometryShaderConstantBufferLayout Shaders::dataToSendToGSConstantBuffer = {};

ID3D11Device* Shaders::device = NULL;
ID3D11DeviceContext* Shaders::deviceContext = NULL;

Shaders::Shaders()
{
}


Shaders::~Shaders()
{
}

void Shaders::Destructor()
{
	delete[] pixelShaders;

	ReleaseMacro(vertexShader);
	ReleaseMacro(shadowVS);
	ReleaseMacro(shadowPS);
	ReleaseMacro(particleVertexShader);
	ReleaseMacro(particleGeometryShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(grayscaleShader);
	ReleaseMacro(sepiaShader);
	ReleaseMacro(inverseShader);

	ReleaseMacro(vsConstantBuffer);
}

void Shaders::Update()
{
	deviceContext->VSSetShader(vertexShader, 0, 0);
	deviceContext->PSSetShader(pixelShaders[activeShader], 0, 0);
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void Shaders::LoadShadersAndInputLayout(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Shaders::device = device;
	Shaders::deviceContext = deviceContext;

	// Load Vertex Shaders --------------------------------------
	LoadVertexShader(L"VertexShader.cso", VERTEX_LAYOUT, &vertexShader);
	LoadVertexShader(L"ShadowVertexShader.cso", SHADOW_LAYOUT, &shadowVS);
	LoadVertexShader(L"ParticleVertexShader.cso", PARTICLE_LAYOUT, &particleVertexShader);

	// Load Geometry Shader -------------------------------------
	LoadGeometryShader(L"ParticleGeometryShader.cso", &particleGeometryShader);

	// Load Pixel Shaders ---------------------------------------
	LoadPixelShader(L"PixelShader.cso", &pixelShader);
	LoadPixelShader(L"GrayscalePixelShader.cso", &grayscaleShader);
	LoadPixelShader(L"SepiaPixelShader.cso", &sepiaShader);
	LoadPixelShader(L"InversePixelShader.cso", &inverseShader);
	LoadPixelShader(L"ShadowPixelShader.cso", &shadowPS);
	LoadPixelShader(L"ParticlePixelShader.cso", &particlePixelShader);

	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth = sizeof(dataToSendToVSConstantBuffer);
	cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = 0;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&vsConstantBuffer));

	D3D11_BUFFER_DESC GSCBufferDesc;
	GSCBufferDesc.ByteWidth = sizeof(dataToSendToGSConstantBuffer);
	GSCBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	GSCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	GSCBufferDesc.CPUAccessFlags = 0;
	GSCBufferDesc.MiscFlags = 0;
	GSCBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&GSCBufferDesc,
		NULL,
		&gsConstantBuffer));

	activeShader = 0;
	pixelShaders = new ID3D11PixelShader*[shaderCount] {
		pixelShader,
			grayscaleShader,
			sepiaShader,
			inverseShader
	};
}

void Shaders::LoadPixelShader(wchar_t* file, ID3D11PixelShader** shader)
{
	ID3DBlob* psBlob;
	D3DReadFileToBlob(file, &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		shader));

	// Clean up
	ReleaseMacro(psBlob);
}

void Shaders::LoadVertexShader(wchar_t* file, LAYOUT inputLayoutType, ID3D11VertexShader** shader)
{
	// Load shader blob
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(file, &vsBlob);

	if (inputLayoutType == VERTEX_LAYOUT)
		InputLayouts::InitializeVertexLayout(device, vsBlob);
	else if (inputLayoutType == PARTICLE_LAYOUT)
		InputLayouts::InitializeParticleLayout(device, vsBlob);
	else
		InputLayouts::InitializeShadowLayout(device, vsBlob);

	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		shader));

	// Clean up
	ReleaseMacro(vsBlob);
}

void Shaders::LoadGeometryShader(wchar_t* file, ID3D11GeometryShader** shader)
{
	ID3DBlob* gsBlob;
	HR(D3DReadFileToBlob(file, &gsBlob));

	// Create the shader on the device
	HR(device->CreateGeometryShader(
		gsBlob->GetBufferPointer(),
		gsBlob->GetBufferSize(),
		NULL,
		&particleGeometryShader));

	// Clean up
	ReleaseMacro(gsBlob);
}
