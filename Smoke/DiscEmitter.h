//Reference to https://community.arm.com/graphics/b/blog/posts/particle-system-realtime-smoke-rendering-with-opengl-es-2-0

#pragma once
#include "Particle.h"
#include <random>

class DiscEmitter {
public:
	void DiscEmitter::getParticle(Particle &part);

private:
	float M_PI = 3.14159265358979323846264338327950288f;
	float discRadius = 2.0f;
	float maxEmissionAngle = 10.0f;
};