#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "Button.h"

class TextBox :
	public Button
{
public:

	TextBox(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* batch, SpriteFont* font, wchar_t* text, int maxSize);
	~TextBox();

	void clear();
	void append(char c);
	void backspace();
	const wchar_t* getWideText();
	const char* getText();
	int length();

	bool active;
	int maxSize;
	void Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData);

private:

	wstring wideText;
	string stringText;
};

#endif