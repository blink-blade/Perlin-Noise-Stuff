#version 460 core
out vec4 FragColor;
in vec3 tileColor;
in vec3 FragPos; 
in vec2 TexCoords;
in float type;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1000
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int pointLightCount;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight; 

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  
  
uniform vec4 timeOffsetColor;
uniform float mixAmount;
uniform vec3 viewPos;
uniform int width;
uniform int height;
uniform Material material;
uniform sampler2D heightTexture;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 0.0;
    if (type == 0.0) {
        shininess = 32.0;
    }
    else if (type == 1.0) {
        shininess = 5.0;
    }
    else {
        shininess = 2.0;
    }
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 0.0;
    if (type == 0.0) {
        shininess = 32.0;
    }
    else if (type == 1.0) {
        shininess = 5.0;
    }
    else {
        shininess = 2.0;
    }
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 


vec3 getNormal() {
    vec2 pos = vec2(int(TexCoords.x * width), int(TexCoords.y * height));
    vec3 bottomLeft = vec3(FragPos.x, texture(heightTexture, vec2(FragPos.x / width, FragPos.z / height)).r, FragPos.z);
    vec3 bottomRight = vec3(FragPos.x + 1, texture(heightTexture, vec2(FragPos.x + 1 / width, FragPos.z / height)).r, FragPos.z);
    vec3 topLeft = vec3(FragPos.x, texture(heightTexture, vec2(FragPos.x / width, FragPos.z + 1 / height)).r, FragPos.z + 1);
    vec3 topRight = vec3(FragPos.x + 1, texture(heightTexture, vec2(FragPos.x + 1 / height, FragPos.z + 1 / height)).r, FragPos.z + 1);


    vec3 U = topLeft - bottomLeft;
	vec3 V = topRight - bottomLeft;

	vec3 normal;
	normal.x = (U.y * V.z) - (U.z * V.y);
	normal.y = (U.z * V.x) - (U.x * V.z);
	normal.z = (U.x * V.y) - (U.y * V.x);

    return normalize(normal);
}
void main() {
    
    vec3 norm = getNormal();

    vec3 viewDir = normalize(viewPos - FragPos);
    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < pointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    FragColor = vec4(result, 1.0);
}