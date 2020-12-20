#include "Models.h"

void Models::drawDrone(glm::mat4 &ModelMatrix, glm::vec3 &cameraPos, float &hoverSpeed, float &hoverAnimation, bool &isHoverUp, bool isHover, float yaw, float pitch, float strafeRotationZ, int camNum, glm::vec3 droneCam[], glm::vec3 controllerPos, glm::vec3 cameraFront,
	glm::vec3 cameraUp, Shader* shader, glm::mat4 &ViewMatrix, ThreeDModel &droneModel, bool isSpotlight, bool isBB, bool isOctree) {

	//Hover animation
	if (isHover) {
		if (hoverAnimation >= 3.0f) {		//Reached top
			isHoverUp = true;
		}
		else if (hoverAnimation <= -3.0f) {	//Reached bottom
			isHoverUp = false;
		}
		//Ascend
		if (hoverAnimation < 0.0f && isHoverUp == false) {
			hoverSpeed += 0.001f;
			cameraPos -= glm::vec3(0.0f, hoverAnimation, 0.0f);
			hoverAnimation += hoverSpeed;
			cameraPos += glm::vec3(0.0f, hoverAnimation, 0.0f);
		}
		else if (hoverAnimation < 3.0f && isHoverUp == false) {
			hoverSpeed -= 0.01f;
			cameraPos -= glm::vec3(0.0f, hoverAnimation, 0.0f);
			hoverAnimation += hoverSpeed;
			cameraPos += glm::vec3(0.0f, hoverAnimation, 0.0f);
		}
		//Descend
		else {
			if (hoverAnimation > 0.0f && isHoverUp == true) {
				hoverSpeed += 0.001f;
				cameraPos -= glm::vec3(0.0f, hoverAnimation, 0.0f);
				hoverAnimation -= hoverSpeed;
				cameraPos += glm::vec3(0.0f, hoverAnimation, 0.0f);
			}
			else if (hoverAnimation > -3.0f && isHoverUp == true) {
				hoverSpeed -= 0.01f;
				cameraPos -= glm::vec3(0.0f, hoverAnimation, 0.0f);
				hoverAnimation -= hoverSpeed;
				cameraPos += glm::vec3(0.0f, hoverAnimation, 0.0f);
			}
		}
	}
	///End of hover animation

	//Matrix transformations
	ModelMatrix = glm::translate(glm::mat4(1.0f), cameraPos);
	ModelMatrix = glm::rotate(ModelMatrix, -yaw - 135, glm::vec3(0.0f, 1.0f, 0.0f));		//Drone yaw
	ModelMatrix = glm::rotate(ModelMatrix, pitch, glm::vec3(1.0f, 0.0f, 0.0f));				//Drone pitch
	ModelMatrix = glm::rotate(ModelMatrix, strafeRotationZ, glm::vec3(0.0f, 0.0f, 1.0f));	//Strafe rotations

	if (camNum == 1 || camNum == 2 || camNum == 3) {	//Main bottom camera
		/*glm::mat4 CameraMatrix = glm::translate(glm::mat4(1.0f), cameraPos);
		CameraMatrix = glm::rotate(CameraMatrix, -yaw - 135, glm::vec3(0.0f, 1.0f, 0.0f));
		CameraMatrix = glm::rotate(CameraMatrix, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		CameraMatrix = glm::rotate(CameraMatrix, strafeRotationZ, glm::vec3(0.0f, 0.0f, 1.0f));*/

		glm::mat4 CameraMatrix = glm::translate(ModelMatrix, droneCam[camNum-1]);

		glm::vec3 newCameraPos = glm::vec3(CameraMatrix * glm::vec4(1.0f));
		ViewMatrix = glm::lookAt(newCameraPos, newCameraPos + cameraFront, cameraUp);
	}
	else if (camNum == 4) {	//Controller camera
		ViewMatrix = glm::lookAt(controllerPos, cameraPos + cameraFront, cameraUp);
	}
	else if (camNum == 5) {	//Bird's view camera
		ViewMatrix = glm::lookAt(cameraPos + glm::vec3(0.0f, 400.0f, 0.0f), cameraPos + cameraFront, cameraUp);
	}
	else if (camNum == 6) {	//Rear facing camera
		glm::mat4 CameraMatrix = glm::translate(ModelMatrix, droneCam[3]);

		glm::vec3 newCameraPos = glm::vec3(CameraMatrix * glm::vec4(1.0f));
		ViewMatrix = glm::lookAt(newCameraPos, newCameraPos - cameraFront, cameraUp);
	}
	shader->setMat4("view", ViewMatrix);
	shader->setMat4("model", ModelMatrix);
	droneModel.drawElementsUsingVBO(shader);

	droneModel.updatePoints(ModelMatrix);
	///End of matrix transformation

	//Drawing bounding boxes
	if (isBB) {
		droneModel.drawBoundingBox(shader);
	}
	if (isOctree) {
		droneModel.drawOctreeLeaves(shader);
	}
	///End of drawing bounding boxes

	//Spotlight
	if (isSpotlight) {	//If spotlight is on, set diffuse to 1 and specular to 0.4
		shader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader->setVec3("spotLight.specular", 0.5f, 0.5f, 0.5f);
	}
	else {	//If spotlight is off, set diffuse and specular to 0
		shader->setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
		shader->setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
	}
	///End of spotlight
}


void Models::drawStaticModel(glm::mat4 &ModelMatrix, glm::vec3 modelPos, float rotate, glm::vec3 rotateAxis, Shader* shader, glm::mat4 &ViewMatrix, ThreeDModel &model, bool isBB, bool isOctree) {
	ModelMatrix = glm::translate(glm::mat4(1.0), modelPos);
	if (rotate != 0) {
		ModelMatrix = glm::rotate(ModelMatrix, rotate, rotateAxis);
	}
	shader->setMat4("view", ViewMatrix);
	shader->setMat4("model", ModelMatrix);
	model.drawElementsUsingVBO(shader);

	if (isBB) {
		model.drawBoundingBox(shader);
	}
	if (isOctree) {
		model.drawOctreeLeaves(shader);
	}
}


void Models::drawDroneShadowMapModel(glm::mat4 &ModelMatrix, glm::vec3 &cameraPos, float yaw, float pitch, float strafeRotationZ, Shader *shader, ThreeDModel &droneModel) {
	//Matrix transformations
	ModelMatrix = glm::translate(glm::mat4(1.0f), cameraPos);
	ModelMatrix = glm::rotate(ModelMatrix, -yaw - 135, glm::vec3(0.0f, 1.0f, 0.0f));		//Drone yaw
	ModelMatrix = glm::rotate(ModelMatrix, pitch, glm::vec3(1.0f, 0.0f, 0.0f));				//Drone pitch
	ModelMatrix = glm::rotate(ModelMatrix, strafeRotationZ, glm::vec3(0.0f, 0.0f, 1.0f));	//Strafe rotations

	shader->setMat4("model", ModelMatrix);
	droneModel.drawElementsUsingVBO(shader);
}


void Models::drawStaticShadowMapModel(glm::mat4 &ModelMatrix, glm::vec3 modelPos, float rotate, glm::vec3 rotateAxis, Shader* shader, ThreeDModel &model) {
	ModelMatrix = glm::translate(glm::mat4(1.0), modelPos);
	if (rotate != 0) {
		ModelMatrix = glm::rotate(ModelMatrix, rotate, rotateAxis);
	}
	shader->setMat4("model", ModelMatrix);
	model.drawElementsUsingVBO(shader);
}