#ifndef SHADERS
#define SHADERS

#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "InputLayouts.h"
#include "Mesh.h"

class Shaders
{
public:
	Shaders();
	~Shaders();

	static void Destructor(void);

	static void LoadShadersAndInputLayout(ID3D11Device*, ID3D11DeviceContext*);
	static void Update(void);

	static ID3D11PixelShader** pixelShaders;
	static ID3D11PixelShader* pixelShader;
	static ID3D11PixelShader* grayscaleShader;
	static ID3D11PixelShader* sepiaShader;
	static ID3D11PixelShader* inverseShader;
	static ID3D11VertexShader* vertexShader;
	static ID3D11VertexShader* particleVertexShader;
	static ID3D11GeometryShader* particleGeometryShader;
	static ID3D11GeometryShader* streamOutGeometryShader;
	static ID3D11PixelShader* particlePixelShader;
	static UINT activeShader;
	static const UINT shaderCount = 4;
	static ID3D11VertexShader* shadowVS;
	static ID3D11PixelShader* shadowPS;

	// Constant buffer info
	static ID3D11Buffer* vsConstantBuffer;
	static ID3D11Buffer* gsConstantBuffer;
	static VertexShaderConstantBufferLayout dataToSendToVSConstantBuffer;
	static GeometryShaderConstantBufferLayout dataToSendToGSConstantBuffer;

private:
	static void LoadPixelShader(wchar_t* file, ID3D11PixelShader** shader);
	static void LoadVertexShader(wchar_t* file, LAYOUT inputLayoutType, ID3D11VertexShader** shader);
	static void LoadGeometryShader(wchar_t* file, ID3D11GeometryShader** shader, bool streamOut = false);

	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
};

#endif