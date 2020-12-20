#version 330
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
    //Get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    //Map to [0,1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;

    //Write as modified depth
    gl_FragDepth = lightDistance;
}