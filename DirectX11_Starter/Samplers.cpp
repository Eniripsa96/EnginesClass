#include "Samplers.h"

ID3D11SamplerState* Samplers::linearSampler = NULL;
ID3D11SamplerState* Samplers::pointSampler = NULL;
ID3D11SamplerState* Samplers::anisotropicSampler = NULL;

Samplers::Samplers()
{
}


Samplers::~Samplers()
{
}

// Creates the samplers used by the game
void Samplers::CreateSamplers(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	// Sample state - linear wrap filtering
	D3D11_SAMPLER_DESC* desc = new D3D11_SAMPLER_DESC();
	desc->MaxLOD = D3D11_FLOAT32_MAX;
	desc->Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(desc, &linearSampler);

	// Sample state - anisotropic wrap filtering
	desc->Filter = D3D11_FILTER_ANISOTROPIC;
	desc->MaxAnisotropy = 16;
	device->CreateSamplerState(desc, &anisotropicSampler);

	// Sample state - point wrap filtering
	desc->Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc->AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc->AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc->AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(desc, &pointSampler);
	delete desc;
}