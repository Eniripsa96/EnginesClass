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
	text.clear();
}

// Appends a char onto the text
void TextBox::append(char c)
{
	wideText += c;
	text += c;
}

// Deletes the last entered character
void TextBox::backspace()
{
	if (length() == 0) return;

	text.pop_back();
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
	return text.c_str();
}

// Retrieves the length of the current text
int TextBox::length()
{
	return text.length();
}