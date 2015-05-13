#include "TextBox.h"


TextBox::TextBox(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* pBatch, SpriteFont* pFont, wchar_t* pText)
	: Button(mesh, mat, pos, pBatch, pFont, pText)
{
}


TextBox::~TextBox()
{
	// This prevents a memory leak
	inputText.clear();
}
