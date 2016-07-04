// Frame.h: interface for the Frame class.
//
//////////////////////////////////////////////////////////////////////

#include "Element.h"

class Frame  
{
	
public:
	int TotalElements;
	Element ** ElementList;
	Frame();
	virtual ~Frame();
	Frame * nextFrame;

};

