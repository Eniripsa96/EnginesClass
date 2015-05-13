#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "Button.h"

class TextBox :
	public Button
{
public:
	TextBox(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* batch, SpriteFont* font, wchar_t* text);
	~TextBox();

	bool active;
	wstring inputText;
};

#endif