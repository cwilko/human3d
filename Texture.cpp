// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Texture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Texture::Texture()
{

	next = NULL;
}

Texture::~Texture()
{

}

bool Texture::SetTextureName(char *name)
{

	if ( strlen(name) >= 256 ) return false;

	strcpy(TextureName, name);

	return true;

}

char * Texture::GetTextureName()
{
	return TextureName;
}
