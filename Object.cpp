// Object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////

#include "Object.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Object::Object()
{
	TotalFrames = 0;
	head = NULL;
	currentFrame = NULL;

}

Object::Object(int TF)
{

	if (! TF )  { 
		
		Object(); 
		return; 
	}

	TotalFrames = TF;
	
	// UP TO TF
	head = new Frame();
	currentFrame = head;

	for (int n = 0; n < TotalFrames - 1; n++ ) {

		currentFrame->nextFrame = new Frame();

		currentFrame = currentFrame->nextFrame;

	}

	currentFrame = head;

}

Object::~Object()
{

}

bool Object::AdvanceFrame()
{


	if (currentFrame->nextFrame == NULL) {
		currentFrame = head;
		return FALSE;
	}
	else
	{
		currentFrame = currentFrame->nextFrame;
		return TRUE;
	}


}

bool Object::LoadTextures(TextureList *Textures, LPDIRECT3DDEVICE3 lpd3dDevice, HWND hwnd)
{

	Texture * newTexture;
	Element ** tempElements;

	int frame, element;

	newTexture = new Texture();

	for ( frame = 0; frame < TotalFrames; frame++ ) {

		tempElements = currentFrame->ElementList;

		for ( element = 0; element < currentFrame->TotalElements; element++ ) {


				newTexture->SetTextureName( tempElements[element]->GetTextureName() );
					
				if (! 
					(tempElements[element]->ElementTexture = Textures->Contains( newTexture ) 
				) ) {
	
					if (! Textures->AddTexture( newTexture ) ) 
						return false;

					tempElements[element]->ElementTexture = newTexture;

					newTexture = new Texture();
				
				}



		}

		AdvanceFrame();


	}



	if (! Textures->LoadAllTextures(lpd3dDevice) ) 
		return false;

	return true;

}
