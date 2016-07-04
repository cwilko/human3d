// Vertex.h: interface for the Vertex class.
//
//////////////////////////////////////////////////////////////////////


class Vertex  
{
public:
	float z;
	float y;
	float x;
	float tu;
	float tv;
	Vertex();
	Vertex(float x, float y, float z);
	virtual ~Vertex();

};
