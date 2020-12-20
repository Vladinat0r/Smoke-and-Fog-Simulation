#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D DiffuseMap;
uniform sampler2D DiffuseMap2;
uniform float mixStr;

out vec4 FragColor;

//Fog - Reference to http://www.terathon.com/lengyel/Lengyel-UnifiedFog.pdf
in vec3 aV;         // (a / 2) * V				//uniform float3
in float c1;        // k * (F dot P + F dot C)
in float c2;        // (1 - 2k) * (F dot P)
in float F_dot_V;


void main()
{
    FragColor = mix(texture(DiffuseMap, TexCoords), texture(DiffuseMap2, TexCoords), mixStr);
    //FragColor = vec4(1.0) * texture(DiffuseMap, TexCoords);

    //Fog
    vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);  //Final color		//float4	//values????
    vec4 fogColor = vec4(0.33f, 0.33f, 0.33f, 1.0f);
    
    //Calculate g(P) using Equation (13)
    float g = min(c2, 0.0);
    g = -length(aV) * (c1 - g * g / abs(F_dot_V));
    
    //Calculate fog fraction and apply
    float f = clamp(exp2(-g), 0.0, 1.0);
    
    FragColor.rgb = FragColor.rgb * f + fogColor.rgb * (1.0 - f);   //Second color.rgb was glFogParameters.color.rgb
    FragColor.a = color.a;
}