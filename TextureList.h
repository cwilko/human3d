// TextureList.h: interface for the TextureList class.
//
//////////////////////////////////////////////////////////////////////

#include "Texture.h"
#include <d3d.h>

class TextureList  
{

public:
	bool LoadAllTextures(LPDIRECT3DDEVICE3 lpd3dDevice);
	
	Texture * Contains( Texture * newTexture );
	bool AddTexture( Texture * newTexture);
	bool Contains( char * );


	TextureList();
	virtual ~TextureList();

	Texture * currentTexture, *head;


private:
	
	int TotalTextures;

	static HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf,VOID* param );
	static HRESULT CopyBitmapToSurface( LPDIRECTDRAWSURFACE4 pddsTarget,HBITMAP hbmBitmap, DWORD dwFlags );
	int LoadTexture(LPDIRECT3DDEVICE3 lpd3dDevice, TCHAR* poo, Texture * pTexture);


	

};

    
