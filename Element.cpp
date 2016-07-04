// Element.cpp: implementation of the Element class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Element.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Element::Element(int TV, int TF)
{
	TotalVertices = TV;
	TotalFaces = TF;	

	VertexList = new Vertex[TotalVertices];
	FaceList = new D3DVERTEX[3 * TotalFaces];

	for (int i = 0; i < 3 * TotalFaces; i++) {

		FaceList[i] = D3DVERTEX ( D3DVECTOR(0,0,0), D3DVECTOR(0,0,0), 0 , 0 );

	}

}



Element::~Element()
{

}


int Element::GetVertexCount()
{

	return TotalVertices;

}

int Element::GetFaceCount()
{

	return TotalFaces;
}

int Element::AssignID(char *name)
{

	if (strlen(name) >= 256) return 0;

	name+=2;
	char * end = name;

	while (*end != '\"') end++;
	*end = '\0';

	strcpy(ID,name);

	return 1;

}

char * Element::GetID()
{

	return ID;

}

int Element::AddTextureName(char *name)
{
	if (strlen(name) >= 256) return 0;

	name++;
	char * end = name;

	while (*end != '\"' && *end != '\0') end++;
	*end = '\0';

	strcpy(TextureName,name);

	return 1;
}

char * Element::GetTextureName()
{

	return TextureName;

}