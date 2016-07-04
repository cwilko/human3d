// Element.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS
#include <d3d.h>
#include "Vertex.h"
#include "Texture.h"



class Element  
{

	int TotalVertices, TotalFaces;

public:
	Texture * ElementTexture;
	
	char * GetTextureName();
	int AddTextureName( char * name);
	D3DVERTEX * FaceList;
	Vertex * VertexList;
	char * GetID();
	int AssignID( char * name);
	int GetFaceCount();
	int GetVertexCount();

	Element( int TotalVertices, int TotalFaces );
	virtual ~Element();

private:
	char TextureName[256];
	char ID[256];
};

