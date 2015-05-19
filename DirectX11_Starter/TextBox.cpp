#include "TextBox.h"


TextBox::TextBox(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* pBatch, SpriteFont* pFont, wchar_t* pText, int maxSize)
	: Button(mesh, mat, pos, pBatch, pFont, pText)
{
	this->maxSize = maxSize;
}


TextBox::~TextBox()
{
	// This prevents a memory leak
	clear();
}

// Clears the string buffers
void TextBox::clear()
{
	wideText.clear();
	stringText.clear();
}

// Appends a char onto the text
void TextBox::append(char c)
{
	wideText += c;
	stringText += c;
	this->SetText(wideText.c_str());
}

// Deletes the last entered character
void TextBox::backspace()
{
	if (length() == 0) return;

	stringText.pop_back();
	wideText.pop_back();
}

// Retrieves the wide text version of the textbox data
const wchar_t* TextBox::getWideText()
{
	return wideText.c_str();
}

// Retrieves the normal text version of the textbox data
const char* TextBox::getText()
{
	return stringText.c_str();
}

// Retrieves the length of the current text
int TextBox::length()
{
	return stringText.length();
}

void TextBox::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData)
{
	rect = RECT{ 0, 0, material->getTexWidth(), material->getTexHeight()};
	batch->Draw(material->resourceView, XMFLOAT2(position.x, position.y), &rect, XMLoadFloat4(&XMFLOAT4(hovered ? 0.5f : 1.5f, 1.5f, 1.5f, 1.0f)), 0.0f, XMFLOAT2(0.0f, .0f), XMFLOAT2(scale.x, scale.y));
	font->DrawString(batch, text, textPos, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale.x, scale.y));
}