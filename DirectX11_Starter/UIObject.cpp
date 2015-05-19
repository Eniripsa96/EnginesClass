#include "UIObject.h"


UIObject::UIObject(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* pBatch, SpriteFont* pFont, wchar_t* pText)
: GameObject(mesh, mat, pos, &XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	batch = pBatch;
	font = pFont;
	SetText(pText);
	rect = RECT{ 0, 0, material->getTexWidth(), material->getTexHeight() };
}

UIObject::~UIObject()
{
}

void UIObject::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData) {
	batch->Draw(material->resourceView, XMFLOAT2(position.x, position.y), &rect, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale.x, scale.y));
	font->DrawString(batch, text, textPos, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale.x, scale.y));
}



void UIObject::Move(float x, float y) {
	position.x += x;
	position.y += y;

	textPos.x += x;
	textPos.y += y;
}

void UIObject::Update(int x, int y) {
	hovered = IsOver(x, y);
	GameObject::Update(0.0f);
}

bool UIObject::IsOver(int x, int y) {
	return x >= position.x && x <= position.x + (scale.x * material->getTexWidth()) && y >= position.y && y <= position.y + (scale.y * material->getTexHeight());
}

void UIObject::SetText(const wchar_t* pText)
{
	text = pText;

	XMVECTOR xmSize = font->MeasureString(text);
	XMFLOAT2 size;
	XMStoreFloat2(&size, xmSize);
	textWidth = size.x;
	textHeight = size.y;

	textPos = XMFLOAT2(position.x + (scale.x * (material->getTexWidth() - size.x)) / 2, position.y + (scale.y * (material->getTexHeight() - size.y)) / 2);
}

void UIObject::Scale(XMFLOAT3* scale)
{
	this->scale.x *= scale->x;
	this->scale.y *= scale->y;
	this->scale.z *= scale->z;

	XMVECTOR xmSize = font->MeasureString(text);
	XMFLOAT2 size;
	XMStoreFloat2(&size, xmSize);
	textWidth = size.x;
	textHeight = size.y;

	textPos = XMFLOAT2(position.x + (scale->x * (material->getTexWidth() - size.x)) / 2, position.y + (scale->y * (material->getTexHeight() - size.y)) / 2);
}
