#include "TextHandler.h"


TextHandler::TextHandler()
{
	this->baseViewMatrix = XMMatrixIdentity();
}

TextHandler::TextHandler(const TextHandler& originalObj)
{

}

TextHandler::~TextHandler()
{

}

bool TextHandler::CreateSentence(ID3D11Device* device, int maxLength)
{

	return true;
}

void TextHandler::ReleaseSentence()
{
	
	return;
}
void TextHandler::RenderSentence(ID3D11DeviceContext*)
{


	return;
}

bool TextHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result;

	return true;
}

void TextHandler::Shutdown()
{

	return;
}

void TextHandler::Render(ID3D11DeviceContext* deviceContext)
{

	return;
}

int TextHandler::GetId()
{
	return this->id;
}
