//Reference to https://community.arm.com/graphics/b/blog/posts/particle-system-realtime-smoke-rendering-with-opengl-es-2-0

#version 330 core

precision mediump float;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};
uniform Material material;


in float v_ageFactor;
in vec3 v_v3colour;

out vec4 FragColor;

void main() {
    float alphaFactor;
    //TexCoords = vec2(gl_PointCoord.x, gl_PointCoord.y);
    
    //Define an alpha modulation factor depending on the particle’s size.
    if(v_ageFactor <= 0.5) {
        alphaFactor = 0.914 * v_ageFactor;
    }
    else {
        alphaFactor = -0.914 * v_ageFactor;
    }

    FragColor = texture2D(material.diffuse, TexCoords);
    FragColor.a = FragColor.a * alphaFactor;
}




/*
//Reference to https://community.arm.com/graphics/b/blog/posts/particle-system-realtime-smoke-rendering-with-opengl-es-2-0

#version 330 core

precision mediump float;

in float v_ageFactor;
in vec3 v_v3colour;

uniform sampler2D smoke_texture;

out vec4 FragColor;

void main() {
    vec4 texColour;
    vec2 texCoords;
    float alphaFactor;
    texCoords = vec2(gl_PointCoord.x, gl_PointCoord.y);
    
    //Define an alpha modulation factor depending on the particle’s size.
    if(v_ageFactor <= 0.5) {
        alphaFactor = 0.14 * v_ageFactor;
    }
    else {
        alphaFactor = -0.14 * v_ageFactor;
    }
    
    //vec4 baseColour = vec4(v_v3colour.x, v_v3colour.y, v_v3colour.z, 1.0);
    texColour = texture2D(smoke_texture, texCoords);
    
    //Modulate alpha component.
    texColour.a = texColour.r * alphaFactor;
    
    //FragColor = vec4(texColour.r, texColour.r, texColour.r, texColour.a) * baseColour;
    FragColor = texture(smoke_texture, texCoords);
    FragColor.a = texColour.a;
}
*/