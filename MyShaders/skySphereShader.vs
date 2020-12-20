#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;  
//out vec3 Normal;
out vec2 TexCoords;
  

//Fog - Reference to http://www.terathon.com/lengyel/Lengyel-UnifiedFog.pdf
uniform vec3 C;			//Camera position

out vec3 aV;		// (a / 2) * V
out float c1;		// k * (F dot P + F dot C)
out float c2;		// (1 - 2k) * (F dot P)
out float F_dot_V;


void main()
{
    gl_Position = projection * view * model * vec4(in_Position, 1.0);
    FragPos = vec3(model * vec4(in_Position, 1.0));
    //Normal = mat3(transpose(inverse(model))) * in_Normal;
	TexCoords = in_TexCoord * 4.0f;

	//Fog
	int k;
	float a = 0.00004f;						//??? Constant density
	vec3 F = vec3(0.0f, 1.0f, 0.0f);		//??? Plane vector

	if (dot(F, C) <= 0) {
		k = 1;
	}
	else {
		k = 0;
	}

	vec3 V = C - FragPos;

	aV = (a / 2) * V;
	c1 = k * (dot(F, FragPos) + dot(F, C));
	c2 = (1 - 2 * k) * (dot(F, FragPos));
	F_dot_V = dot(F, V);
}


//F - Plane vector
//P - FragPos			???
//C - Camera position
//V = C-P - The distance between camera and fragment point