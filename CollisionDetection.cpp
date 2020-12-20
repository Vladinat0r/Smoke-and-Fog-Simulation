#include "CollisionDetection.h"

bool CollisionDetection::modelAABBCollision(ThreeDModel &model1, glm::mat4 modelMatrix1, ThreeDModel &model2, glm::mat4 modelMatrix2, float adjustMaxX2, float adjustMaxY2, float adjustMaxZ2) {
	//Model1
	glm::vec4 LTF1 = model1.getLTF(modelMatrix1); //Left  Top    Front point
	glm::vec4 LTB1 = model1.getLTB(modelMatrix1); //Left  Top    Back  point
	glm::vec4 LBF1 = model1.getLBF(modelMatrix1); //Left  Bottom Front point
	glm::vec4 LBB1 = model1.getLBB(modelMatrix1); //Left  Bottom Back  point
	glm::vec4 RTF1 = model1.getRTF(modelMatrix1); //Right Top    Front point
	glm::vec4 RTB1 = model1.getRTB(modelMatrix1); //Right Top    Back  point
	glm::vec4 RBF1 = model1.getRBF(modelMatrix1); //Right Bottom Front point
	glm::vec4 RBB1 = model1.getRBB(modelMatrix1); //Right Bottom Back  point

	float minX1 = 999999;
	float minY1 = 999999;
	float minZ1 = 999999;
	float maxX1 = -999999;
	float maxY1 = -999999;
	float maxZ1 = -999999;

	glm::vec4 Points1[] = {
		LTF1, LTB1, LBF1, LBB1, 
		RTF1, RTB1, RBF1, RBB1
	};

	for (size_t i = 0; i <= Points1->length(); i++) {
		//X
		if (Points1[i].x < minX1) {
			minX1 = Points1[i].x;
		}
		else if (Points1[i].x > maxX1) {
			maxX1 = Points1[i].x;
		}

		//Y
		if (Points1[i].y < minY1) {
			minY1 = Points1[i].y;
		}
		else if (Points1[i].y > maxY1) {
			maxY1 = Points1[i].y;
		}

		//Z
		if (Points1[i].z < minZ1) {
			minZ1 = Points1[i].z;
		}
		else if (Points1[i].z > maxZ1) {
			maxZ1 = Points1[i].z;
		}
	}


	//Model2
	glm::vec4 LTF2 = model2.getLTF(modelMatrix2); //Left  Top    Front point
	glm::vec4 LTB2 = model2.getLTB(modelMatrix2); //Left  Top    Back  point
	glm::vec4 LBF2 = model2.getLBF(modelMatrix2); //Left  Bottom Front point
	glm::vec4 LBB2 = model2.getLBB(modelMatrix2); //Left  Bottom Back  point
	glm::vec4 RTF2 = model2.getRTF(modelMatrix2); //Right Top    Front point
	glm::vec4 RTB2 = model2.getRTB(modelMatrix2); //Right Top    Back  point
	glm::vec4 RBF2 = model2.getRBF(modelMatrix2); //Right Bottom Front point
	glm::vec4 RBB2 = model2.getRBB(modelMatrix2); //Right Bottom Back  point

	float minX2 = 999999;
	float minY2 = 999999;
	float minZ2 = 999999;
	float maxX2 = -999999;
	float maxY2 = -999999;
	float maxZ2 = -999999;

	glm::vec4 Points2[] = {
		LTF2, LTB2, LBF2, LBB2,
		RTF2, RTB2, RBF2, RBB2
	};

	for (size_t i = 0; i < Points2->length(); i++) {
		//X
		if (Points2[i].x < minX2) {
			minX2 = Points2[i].x;
		}

		//Y
		if (Points2[i].y < minY2) {
			minY2 = Points2[i].y;
		}

		//Z
		if (Points2[i].z < minZ2) {
			minZ2 = Points2[i].z;
		}
	}

	maxX2 = minX2 + model2.theBBox.boxWidthX + adjustMaxX2;
	maxY2 = minY2 + model2.theBBox.boxWidthY + adjustMaxY2;
	maxZ2 = minZ2 + model2.theBBox.boxWidthZ + adjustMaxZ2;

	return	(minX1 <= maxX2 && maxX1 >= minX2) &&
			(minY1 <= maxY2 && maxY1 >= minY2) &&
			(minZ1 <= maxZ2 && maxZ1 >= minZ2);
}

float CollisionDetection::getMinZ(ThreeDModel &model, glm::mat4 modelMatrix, float adjustX) {	//Get back face z value
	glm::vec4 LTF = model.getLTF(modelMatrix); //Left  Top    Front point
	glm::vec4 LTB = model.getLTB(modelMatrix); //Left  Top    Back  point
	glm::vec4 LBF = model.getLBF(modelMatrix); //Left  Bottom Front point
	glm::vec4 LBB = model.getLBB(modelMatrix); //Left  Bottom Back  point
	glm::vec4 RTF = model.getRTF(modelMatrix); //Right Top    Front point
	glm::vec4 RTB = model.getRTB(modelMatrix); //Right Top    Back  point
	glm::vec4 RBF = model.getRBF(modelMatrix); //Right Bottom Front point
	glm::vec4 RBB = model.getRBB(modelMatrix); //Right Bottom Back  point

	glm::vec4 Points[] = {
		LTF, LTB, LBF, LBB,
		RTF, RTB, RBF, RBB
	};

	float minZ = 999999;

	for (size_t i = 0; i < Points->length(); i++) {
		//Z
		if (Points[i].z < minZ) {
			minZ = Points[i].z;
		}
	}
	return minZ+adjustX;
}


float CollisionDetection::getMinX(ThreeDModel &model, glm::mat4 modelMatrix, float adjustX) {	//Get back face z value
	glm::vec4 LTF = model.getLTF(modelMatrix); //Left  Top    Front point
	glm::vec4 LTB = model.getLTB(modelMatrix); //Left  Top    Back  point
	glm::vec4 LBF = model.getLBF(modelMatrix); //Left  Bottom Front point
	glm::vec4 LBB = model.getLBB(modelMatrix); //Left  Bottom Back  point
	glm::vec4 RTF = model.getRTF(modelMatrix); //Right Top    Front point
	glm::vec4 RTB = model.getRTB(modelMatrix); //Right Top    Back  point
	glm::vec4 RBF = model.getRBF(modelMatrix); //Right Bottom Front point
	glm::vec4 RBB = model.getRBB(modelMatrix); //Right Bottom Back  point

	glm::vec4 Points[] = {
		LTF, LTB, LBF, LBB,
		RTF, RTB, RBF, RBB
	};

	float minX = 999999;

	for (size_t i = 0; i < Points->length(); i++) {
		//Z
		if (Points[i].x < minX) {
			minX = Points[i].x;
		}
	}
	return minX + adjustX;
}

float CollisionDetection::getMaxX(ThreeDModel &model, glm::mat4 modelMatrix, float adjustX) {	//Get front face z value
	glm::vec4 LTF = model.getLTF(modelMatrix); //Left  Top    Front point
	glm::vec4 LTB = model.getLTB(modelMatrix); //Left  Top    Back  point
	glm::vec4 LBF = model.getLBF(modelMatrix); //Left  Bottom Front point
	glm::vec4 LBB = model.getLBB(modelMatrix); //Left  Bottom Back  point
	glm::vec4 RTF = model.getRTF(modelMatrix); //Right Top    Front point
	glm::vec4 RTB = model.getRTB(modelMatrix); //Right Top    Back  point
	glm::vec4 RBF = model.getRBF(modelMatrix); //Right Bottom Front point
	glm::vec4 RBB = model.getRBB(modelMatrix); //Right Bottom Back  point

	glm::vec4 Points[] = {
		LTF, LTB, LBF, LBB,
		RTF, RTB, RBF, RBB
	};

	float minX = 999999;
	float maxX = -999999;

	for (size_t i = 0; i < Points->length(); i++) {
		//Z
		if (Points[i].x < minX) {
			minX = Points[i].x;
		}
		else if (Points[i].x > maxX) {
			maxX = Points[i].x;
		}
	}
	return maxX = minX + model.theBBox.boxWidthX + adjustX;
}

float CollisionDetection::getMaxZ(ThreeDModel &model, glm::mat4 modelMatrix, float adjustZ) {	//Get front face z value
	glm::vec4 LTF = model.getLTF(modelMatrix); //Left  Top    Front point
	glm::vec4 LTB = model.getLTB(modelMatrix); //Left  Top    Back  point
	glm::vec4 LBF = model.getLBF(modelMatrix); //Left  Bottom Front point
	glm::vec4 LBB = model.getLBB(modelMatrix); //Left  Bottom Back  point
	glm::vec4 RTF = model.getRTF(modelMatrix); //Right Top    Front point
	glm::vec4 RTB = model.getRTB(modelMatrix); //Right Top    Back  point
	glm::vec4 RBF = model.getRBF(modelMatrix); //Right Bottom Front point
	glm::vec4 RBB = model.getRBB(modelMatrix); //Right Bottom Back  point

	glm::vec4 Points[] = {
		LTF, LTB, LBF, LBB,
		RTF, RTB, RBF, RBB
	};

	float minZ = 999999;
	float maxZ = -999999;

	for (size_t i = 0; i < Points->length(); i++) {
		//Z
		if (Points[i].z < minZ) {
			minZ = Points[i].z;
		}
		else if (Points[i].y > maxZ) {
			maxZ = Points[i].y;
		}
	}
	return maxZ = minZ + model.theBBox.boxWidthZ + adjustZ;
}


bool CollisionDetection::pointToAABBCollision(glm::vec3 pos, glm::vec3 box[]) {
	return	(pos.x >= box[0].x && pos.x <= box[1].x) &&
			(pos.y >= box[0].y && pos.y <= box[4].y) &&
			(pos.z <= box[0].z && pos.z >= box[2].z);
}