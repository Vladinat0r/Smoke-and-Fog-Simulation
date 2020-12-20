//Reference to https://community.arm.com/graphics/b/blog/posts/particle-system-realtime-smoke-rendering-with-opengl-es-2-0

#include "DiscEmitter.h"

void DiscEmitter::getParticle(Particle &part)
{
	//Generate a random number in the interval [0,1].
	float rad = (float)rand() / ((float)RAND_MAX);

	//Generate a random number in the interval [0,1].
	float polarAngle = (float)rand() / ((float)RAND_MAX);
	polarAngle *= 2 * M_PI;
	part.initPos.x = discRadius * rad * cos(polarAngle);
	part.initPos.y = discRadius * rad * sin(polarAngle);
	part.initPos.z = discRadius * rad * cos(polarAngle);

	//Generate a random number in the interval [0,1].
	float azimuthAngle = (float)rand() / ((float)RAND_MAX);
	azimuthAngle *= maxEmissionAngle;
	part.initVel.x = sin(azimuthAngle) * cos(polarAngle) * 20;
	part.initVel.y = sin(azimuthAngle) * sin(polarAngle) * 25;
	part.initVel.z = cos(azimuthAngle) * sin(polarAngle) * 20;
	part.lifetime = (float)rand() / ((float)RAND_MAX) + 2;

	//Generate a random number in the interval [0,1].
	part.delay = (float)rand() / ((float)RAND_MAX);
}