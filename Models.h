#pragma once
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "shaders/Shader.h"
#include "3DStruct\threeDModel.h"

class Models {
public:
	void drawDrone(glm::mat4 &ModelMatrix, glm::vec3 &cameraPos, float &hoverSpeed, float &hoverAnimation, bool &isHoverUp, bool isHover, float yaw, float pitch, float strafeRotationZ, int camNum, glm::vec3 droneCam[], glm::vec3 controllerPos, glm::vec3 cameraFront,
		glm::vec3 cameraUp, Shader* lightingShader, glm::mat4 &ViewMatrix, ThreeDModel &droneModel, bool isSpotlight, bool isBB, bool isOctree);

	void drawStaticModel(glm::mat4 &ModelMatrix, glm::vec3 modelPos, float rotate, glm::vec3 rotateAxis, Shader* shader, glm::mat4 &ViewMatrix, ThreeDModel &model, bool isBB, bool isOctree);


	void drawDroneShadowMapModel(glm::mat4 & ModelMatrix, glm::vec3 & cameraPos, float yaw, float pitch, float strafeRotationZ, Shader * shader, ThreeDModel & droneModel);
	void drawStaticShadowMapModel(glm::mat4 & ModelMatrix, glm::vec3 modelPos, float rotate, glm::vec3 rotateAxis, Shader * shader, ThreeDModel & model);
};