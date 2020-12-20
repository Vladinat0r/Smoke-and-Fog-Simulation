#ifndef _BoundingBox_h_
#define _BoundingBox_h_

#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\type_ptr.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"

#include <windows.h>
#include "../Structures/Vector3d.h"

class BoundingBox
{
public:
	Vector3d	centrePoint;
	float		boxWidthX;
	float		boxWidthY;
	float		boxWidthZ;

	BoundingBox();
	BoundingBox(Vector3d newCen, float newWidthX, float newWidthY, float newWidthZ);
	
	float getLargestExtent();

	static BoundingBox combineTwoBBoxes(BoundingBox &box1, BoundingBox &box2);

	void makeSquareXZ();

	float left() const; //this means the member function will not modify the object.
	float right() const;
	float bottom() const;
	float top() const;
	float front() const;
	float back() const;
};

#endif