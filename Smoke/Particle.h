//Reference to https://community.arm.com/graphics/b/blog/posts/particle-system-realtime-smoke-rendering-with-opengl-es-2-0

#pragma once
#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\type_ptr.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"

class Particle {
public:
	glm::vec3 initPos;
	glm::vec3 posShift;
	glm::vec3 initVel;
	float delay;
	float lifetime;
	float age;

private:

};