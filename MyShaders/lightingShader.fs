//Reference to Blinn-Phong lighting - https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
//Reference to fog - http://www.terathon.com/lengyel/Lengyel-UnifiedFog.pdf

#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpaceDir;
in vec4 FragPosLightSpaceSpotlight;

uniform vec3 viewPos;
uniform sampler2D DiffuseMap;

uniform sampler2D ShadowMapDir;
uniform samplerCube ShadowCubeMap;
uniform sampler2D ShadowMapSpotlight;

uniform float far_plane;

out vec4 FragColor;

//Point light shadows - sample directions
vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

//Fog
in vec3 aV;         // (a / 2) * V
in float c1;        // k * (F dot P + F dot C)
in float c2;        // (1 - 2k) * (F dot P)
in float F_dot_V;

//STRUCTS
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 
uniform Material material;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;

    //Attenuation
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;  //Light position
    vec3 direction; //Spotlight direction

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //Attenuation
    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};
uniform SpotLight spotLight;


float ShadowCalculationDirSpot(vec4 FragPosLightSpaceDir, sampler2D ShadowMap, vec3 normal, vec3 lightDir);
float ShadowCalculationPoint(vec3 fragPos, PointLight light);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 FragPos, vec3 viewDir);


float ShadowCalculationDirSpot(vec4 FragPosLightSpace, sampler2D ShadowMap, vec3 normal, vec3 lightDir) {
    //Perform perspective division
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;

    //Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    //Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(ShadowMap, projCoords.xy).r; 

    //Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    //Solves over-sampling issue by returning 0 for areas beyond the far plane
    if(projCoords.z > 1.0) {
        return 0.0;
    }

    //Check if current FragPos is in shadow
    float bias = max(0.0000005f * (1.0 - dot(normal, lightDir)), 0.0000005f);

	float shadow = 0.0;

	//Percentage-closer filtering
    vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    //float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}


float ShadowCalculationPoint(vec3 fragPos, PointLight light) {
    vec3 fragToLight = fragPos - light.position;
   
    float currentDepth = length(fragToLight);

    float shadow = 0.0f;
    float bias = 0.5f;
    float samples = 9.0f;
    float viewDistance = length(viewPos - fragPos);
	//diskRadius adds an area to fragToLight for sampling from the cubemap based on distance to the viewer
    float diskRadius = (1.0f - (viewDistance / far_plane)) / 25.0f;

	//Percentage-closer filtering
    for (int i = 0; i < samples; ++i) {
        float closestDepth = texture(ShadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;
        if (currentDepth - bias > closestDepth) {
            shadow += 1.0f;
        }
    }

    shadow /= samples;
    return shadow;
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);	//The halfway vector
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	
    float shadow = ShadowCalculationDirSpot(FragPosLightSpaceDir, ShadowMapDir, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(material.diffuse, TexCoords).rgb;
    return lighting;
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);	//The halfway vector
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;

    //Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    float shadow = ShadowCalculationPoint(fragPos, light);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(material.diffuse, TexCoords).rgb;
    return lighting;
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);	//The halfway vector
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));

    //Specular
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    diffuse *= intensity;
    specular *= intensity;

    float shadow = ShadowCalculationDirSpot(FragPosLightSpaceSpotlight, ShadowMapSpotlight, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(material.diffuse, TexCoords).rgb;
    return lighting;
}


void main() {
    //Properties
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, normal, viewDir);

    //Phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);    
    }

    //Phase 3: Spot light
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0) * texture(DiffuseMap, TexCoords);

	//Phase 4: Fog
    vec4 fogColor = vec4(0.33f, 0.33f, 0.33f, 1.0f);
    //Calculate g(P) using Equation (13)
	float g = min(c2, 0.0);
	g = -length(aV) * (c1 - g * g / abs(F_dot_V));
    //Calculate fog fraction and apply
	float f = clamp(exp2(-g), 0.0, 1.0);
	FragColor.rgb = FragColor.rgb * f + fogColor.rgb * (1.0 - f);
}
