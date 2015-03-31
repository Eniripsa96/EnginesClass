#ifndef SAMPLERS_H
#define SAMPLERS_H

#include <d3d11.h>

class Samplers
{
public:
	Samplers();
	~Samplers();

	static void CreateSamplers(ID3D11Device*, ID3D11DeviceContext*);

	static ID3D11SamplerState* linearSampler;
	static ID3D11SamplerState* pointSampler;
	static ID3D11SamplerState* anisotropicSampler;
};

#endif