

#define D3D_OVERLOADS

#include <fstream.h>
#include "Frame.h"


int ProcessVertices(Element *);
int ProcessFaces(Element *);

#define ERR 0
#define SUCCESS 1

#define MAX_ELEMENTS 100

ifstream fin;


extern int LoadFile( char * filename, Frame * frame, int ElementCount) {

	char		string[256];	

	char		delim[] = ":\n\t,";

	char *		token;

	Element *	ElementList[MAX_ELEMENTS];
	char		ElementName[256];
	bool		ElementFlag = false;
	int			TotalElements = 0;
	int			TotalVertices, TotalFaces;

	
	fin.open(filename);

	while ( !fin.getline(string,256).eof() && TotalElements < ElementCount) {
	
		token = strtok(string, delim);
	

		while( token != NULL ) {


			if (!strcmp(token, "Named Element")) {


				token = strtok(NULL,delim);
				strcpy(ElementName, token);
			}
			
									
			if ( !strcmp( token, "Tri-mesh") ) {

					token = strtok(NULL, delim);
					token = strtok(NULL, delim);

					TotalVertices = atoi(token);

					token = strtok(NULL, delim);

					TotalFaces = atoi(token);

					ElementList[TotalElements] = new Element(TotalVertices, TotalFaces);
						
					ElementList[TotalElements]->AssignID(ElementName);
						
					cout << "Start of Element: " << ElementList[TotalElements]->GetID() << endl;

					ElementFlag = true;

			}
				


			if (!strcmp(token, "Vertex list") && ElementFlag)  {
				
				if ( ! ProcessVertices(ElementList[TotalElements]) ) {
					
					cout << "Error in File - Cannot Create Elements" << endl;
					return ERR;
				
				}


			}

			if (!strcmp(token, "Face list") && ElementFlag)  {
				
				if ( ! ProcessFaces(ElementList[TotalElements]) ) {
					
					cout << "Error in File - Cannot Create Elements" << endl;
					return ERR;
				}
				
				cout << "End of Element: " << ElementList[TotalElements]->GetID() << endl;
				
				TotalElements++;

				if (TotalElements >= MAX_ELEMENTS) return ERR;
				
				ElementFlag = false;

			}

			token = strtok(NULL, delim);
		
			
		}	
	}

	fin.close();

	if (! TotalElements) return ERR;

	frame->ElementList = new Element*[TotalElements];

	for ( int n = 0; n < TotalElements; n++ ) {

		frame->ElementList[n] = ElementList[n];

	}

	frame->TotalElements = TotalElements;

	return TotalElements;
}


int ProcessVertices( Element * thisElement ) {
	

	char string[256];

	char delim[] = ": \n\t";

	char * token;

	int VertexNo;

	while ( !fin.getline(string,256).eof() ) {
	
		token = strtok(string, delim);	
	
		while( token != NULL ) {



			if (!strcmp(token, "Vertex")) {
	

				// Find the Number of the current Vertex int the file

				if (token = strtok(NULL, delim)) {

					VertexNo = atoi(token);
			
				} else return ERR;


				// Retrieve the X Value

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "X")) {

						if (token = strtok(NULL, delim)) {

							thisElement->VertexList[VertexNo].x = (float) atof(token);

						} else return ERR;
				
					} else return ERR;

				} else return ERR;


				// Retrieve the Y Value

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "Y")) {

						if (token = strtok(NULL, delim)) {

							thisElement->VertexList[VertexNo].y = (float) atof(token);

						} else return ERR;
				
					} else return ERR;

				} else return ERR;
			

				// Retrieve the Z Value

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "Z")) {

						if (token = strtok(NULL, delim)) {

							thisElement->VertexList[VertexNo].z = (float) atof(token);

						} else return ERR;
				
					} else return ERR;

				} else return ERR;

				// Retrieve the U Value for Texture Mapping (If there is one)

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "U")) {

						if (token = strtok(NULL, delim)) {

							
							thisElement->VertexList[VertexNo].tu = (float) atof(token);

						} else return ERR;
				
					} else return ERR;

				} 

				// Retrieve the V Value for Texture Mapping (If there is one)

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "V")) {

						if (token = strtok(NULL, delim)) {

						
							thisElement->VertexList[VertexNo].tv = (float) atof(token);

						} else return ERR;
				
					} else return ERR;

				}

				cout << "Vertex " << VertexNo;

				cout << 
					"\tX:" << thisElement->VertexList[VertexNo].x << 
					"\tY:" << thisElement->VertexList[VertexNo].y << 
					"\tZ:" << thisElement->VertexList[VertexNo].z << endl;

				if ( ++VertexNo == thisElement->GetVertexCount() ) return SUCCESS;

			} else {

				token = strtok(NULL, delim);

			}

		}



	}

	return SUCCESS;

}

int ProcessFaces( Element * thisElement) {
	

	char string[256];

	char delim[] = ": \n\t";

	char * token;

	int FaceCount = 0;

	int FaceNo;

	int VertexA, VertexB, VertexC;

	
	while ( !fin.getline(string,256).eof() ) {
	
		token = strtok(string, delim);	
	
		while( token != NULL ) {



			if (!strcmp(token, "Face")) {
	

				// Find the Number of the current Vertex int the file

				if (token = strtok(NULL, delim)) {

					FaceNo = atoi(token);			
			
				} else return ERR;


				// Retrieve the X Value

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "A")) {

						if (token = strtok(NULL, delim)) {
	
							VertexA = atoi(token);

						} else return ERR;
				
					} else return ERR;

				} else return ERR;


				// Retrieve the Y Value

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "B")) {

						if (token = strtok(NULL, delim)) {

							VertexB = atoi(token);

						} else return ERR;
				
					} else return ERR;

				} else return ERR;
			

				// Retrieve the Z Value

				if (token = strtok(NULL, delim)) {

					if (!strcmp(token, "C")) {

						if (token = strtok(NULL, delim)) {

							VertexC = atoi(token);

						} else return ERR;
				
					} else return ERR;

				} else return ERR;


				thisElement->FaceList[FaceCount].x	=	thisElement->VertexList[VertexA].x;
				thisElement->FaceList[FaceCount].y	=	thisElement->VertexList[VertexA].y;
				thisElement->FaceList[FaceCount].z	=	thisElement->VertexList[VertexA].z;
				thisElement->FaceList[FaceCount].tu	=	thisElement->VertexList[VertexA].tu;
				thisElement->FaceList[FaceCount].tv	=	thisElement->VertexList[VertexA].tv;
				FaceCount++;

				thisElement->FaceList[FaceCount].x	=	thisElement->VertexList[VertexB].x;
				thisElement->FaceList[FaceCount].y	=	thisElement->VertexList[VertexB].y;
				thisElement->FaceList[FaceCount].z	=	thisElement->VertexList[VertexB].z;
				thisElement->FaceList[FaceCount].tu	=	thisElement->VertexList[VertexB].tu;
				thisElement->FaceList[FaceCount].tv	=	thisElement->VertexList[VertexB].tv;
				FaceCount++;

				thisElement->FaceList[FaceCount].x	=	thisElement->VertexList[VertexC].x;
				thisElement->FaceList[FaceCount].y	=	thisElement->VertexList[VertexC].y;
				thisElement->FaceList[FaceCount].z	=	thisElement->VertexList[VertexC].z;
				thisElement->FaceList[FaceCount].tu	=	thisElement->VertexList[VertexC].tu;
				thisElement->FaceList[FaceCount].tv	=	thisElement->VertexList[VertexC].tv;
				FaceCount++;

				
				cout << "Face " << FaceNo;

				cout << "\tA:" << VertexA << "\tB:" << VertexB << "\tC:" << VertexC << endl;



				if ( ++FaceNo == thisElement->GetFaceCount() ) return SUCCESS;

			} 

			if (!strcmp(token, "Material")) {
			
				token = strtok(NULL, delim);
				

				thisElement->AddTextureName(token);
			
			}

			token = strtok(NULL, delim);

			

		}



	}

	return SUCCESS;

}