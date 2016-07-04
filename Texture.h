// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include <d3d.h>
#include <ddraw.h>

#if !defined( TEXTURE_INCLUDED)
#define TEXTURE_INCLUDED

class Texture  
{
public:
	char * GetTextureName();

	bool SetTextureName( char * name );


	HBITMAP              hbmBitmap;      // Bitmap containing texture image
    LPDIRECTDRAWSURFACE4 pddsSurface;    // Surface of the texture
    LPDIRECT3DTEXTURE2   ptexTexture;    // Direct3D texture for the texture
    
	Texture * next;

	Texture();
	virtual ~Texture();

private:

	
	char TextureName[256];
};

#endif