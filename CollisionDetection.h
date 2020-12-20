#pragma once
#include "3dStruct\BoundingBox.h"
#include "3dStruct\threeDModel.h"


class CollisionDetection {
public:
	bool modelAABBCollision(ThreeDModel &model1, glm::mat4 model1Matrix, ThreeDModel &model2, glm::mat4 model2Matrix, float adjustMaxX2, float adjustMaxY2, float adjustMaxZ2);
	float getMinZ(ThreeDModel & model, glm::mat4 modelMatrix, float adjustX);
	float getMinX(ThreeDModel & model, glm::mat4 modelMatrix, float adjustX);
	float getMaxX(ThreeDModel & model, glm::mat4 modelMatrix, float adjustX);
	float getMaxZ(ThreeDModel & model, glm::mat4 modelMatrix, float adjustX);
	bool pointToAABBCollision(glm::vec3 pos, glm::vec3 box[]);
};