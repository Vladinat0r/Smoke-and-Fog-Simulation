//Refer to https://community.arm.com/graphics/b/blog/posts/particle-system-realtime-smoke-rendering-with-opengl-es-2-0

#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 Position;
uniform vec3 Velocity;
uniform vec3 ParticleTimes;
uniform vec3 cameraPos;
uniform vec3 gravity;
uniform vec3 colour;

out vec3 FragPos;  
out vec3 Normal;
out vec2 TexCoords;

out float v_ageFactor;
out vec3 v_v3colour;
  
void main() {
	vec3 newPos;
	float ageFactor;
	float delay		= ParticleTimes.x;
	float lifetime	= ParticleTimes.y;
	float age		= ParticleTimes.z;

	if (age > delay) {
		//Particle motion equation.
		newPos = in_Position + Position + Velocity * (age - delay) + 0.5 * gravity * (age - delay) * (age - delay);
		ageFactor = 1.0 - ((age - delay) / lifetime);
		ageFactor = clamp(ageFactor, 0.0, 1.0);

		////The older the particle the smaller its size.
		//gl_PointSize = ageFactor * 250.0;
	}
	else {
		newPos = in_Position + Position;

		////If null size, particle will not be drawn.
		//gl_PointSize = 0.0;
		ageFactor = 0.0;
	}

	//Initializing output variables.
	v_ageFactor = ageFactor;
	v_v3colour = colour;

	//Particle position.
	gl_Position.xyz = newPos;
	gl_Position.w = 1.0;

	


	//Initializing output variables.
    FragPos = vec3(model * gl_Position);
    Normal = mat3(transpose(inverse(model))) * in_Normal;
	TexCoords = in_TexCoord;

	//Apply matrix transformation.
	gl_Position = projection * view * model * gl_Position;
}




/*
uniform vec3 Position;
uniform vec3 Velocity;
uniform vec3 ParticleTimes;

uniform mat4 Model;
uniform mat4 ViewPerspective;
uniform vec3 cameraPos;
uniform vec3 gravity;
uniform vec3 colour;

out float v_ageFactor;
out vec3 v_v3colour;

void main() {
	vec3 newPos;
	float ageFactor;
	float delay		= ParticleTimes.x;
	float lifetime	= ParticleTimes.y;
	float age		= ParticleTimes.z;


	if (age > delay) {
		//Particle motion equation.
		newPos = Position + Velocity * (age - delay) + 0.5 * gravity * (age - delay) * (age - delay);
		ageFactor = 1.0 - ((age - delay) / lifetime);
		ageFactor = clamp(ageFactor, 0.0, 1.0);

		//The older the particle the smaller its size.
		gl_PointSize = ageFactor * 250.0;
	}
	else {
		newPos = Position;

		//If null size, particle will not be drawn.
		gl_PointSize = 0.0;
		ageFactor = 0.0;
	}

	//Initializing output variables.
	v_ageFactor = ageFactor;
	v_v3colour = colour;

	//Particle position.
	gl_Position.xyz = newPos;
	gl_Position.w = 1.0;

	////Distance scale
	//float farPlane = 2000.0f;
	//float cameraDist = distance(gl_Position.xyz, cameraPos);
    //float pointScale = 1.0 - (cameraDist / farPlane);
    //pointScale = max(pointScale, 0.15f);	//Minimum scale
    //pointScale = min(pointScale, 1.0f);		//Maximum scale
    //gl_PointSize = gl_PointSize * pointScale;

	//Apply matrix transformation.
	gl_Position = Model * ViewPerspective * gl_Position;
}
*/