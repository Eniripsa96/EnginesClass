#include "Button.h"


Button::Button(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* pBatch, SpriteFont* pFont, wchar_t* pText)
: UIObject(mesh, mat, pos, pBatch, pFont, pText) 
{
	
}

Button::~Button() { }

// Draws the button a different color when hovered over
void Button::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData) 
{
	rect = RECT{ 0, 0, material->getTexWidth(), material->getTexHeight() };
	batch->Draw(material->resourceView, XMFLOAT2(position.x, position.y), &rect, XMLoadFloat4(&XMFLOAT4(hovered ? 0.5f : 1.0f, 1.0f, 1.0f, 1.0f)), 0.0f, XMFLOAT2(0.0f, .0f), XMFLOAT2(scale.x, scale.y));
	font->DrawString(batch, text, textPos, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale.x, scale.y));
}
