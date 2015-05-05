//#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "DirectXGame.h"
//#include "MyDemoGame.h"
//#include <DirectXMath.h>
#include <DirectXMath.h>
#include "WICTextureLoader.h"

using namespace DirectX;

class Material
{
public:
	Material(ID3D11Device*, ID3D11DeviceContext*, ID3D11VertexShader*, ID3D11PixelShader*, ID3D11SamplerState*, const wchar_t*, ID3D11GeometryShader* gs = NULL);
	~Material();

	void Draw();
	void SetShaders();

	UINT getTexWidth();
	UINT getTexHeight();

	ID3D11ShaderResourceView* resourceView;

private:
	UINT texWidth;
	UINT texHeight;

	ID3D11DeviceContext* deviceContext;
	ID3D11Device* device;

	ID3D11SamplerState* samplerState;

	// Shaders
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;
	ID3D11VertexShader* vertexShader;

public:

public:

	static ID3D11ShaderResourceView* Material::CreateRandomTex(ID3D11Device* device)
	{
		XMFLOAT4 randomValues[1024];

		for (int i = 0; i < 1024; i++)
		{
			randomValues[i].x = (rand() % 200) / 100.0f - 1.0f;
			randomValues[i].y = (rand() % 200) / 100.0f - 1.0f;
			randomValues[i].z = (rand() % 200) / 100.0f - 1.0f;
			randomValues[i].w = (rand() % 200) / 100.0f - 1.0f;
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = randomValues;
		initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
		initData.SysMemSlicePitch = 0;

		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		ID3D11Texture1D* randomTex = 0;
		HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* randomTexSRV = 0;
		HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

		ReleaseMacro(randomTex);

		return randomTexSRV;
	}
};

#endif