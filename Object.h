// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////


#include "Frame.h"	// Added by ClassView
#include "TextureList.h"
#include <d3d.h>

class Object  
{
public:
	bool LoadTextures( TextureList * Textures , LPDIRECT3DDEVICE3 lpd3dDevice, HWND hwnd);
	bool AdvanceFrame();
	int TotalFrames;
	Frame * head;
	Frame * currentFrame;
	Object();
	Object(int TotalFrames);
	virtual ~Object();

};

