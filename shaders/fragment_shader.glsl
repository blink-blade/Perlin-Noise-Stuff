#version 460 core
out vec4 FragColor;
in vec3 FragPos; 
in vec3 Normal; 
in vec2 uvs;
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
  
uniform vec3 viewPos;
uniform int width;
uniform int height;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess;
    if (type == 0.0) {
        shininess = 10.0;
        
    }
    else if (type == 1.0) {
        shininess = 5.0;
    }
    else {
        shininess = 2.0;
    }
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, uvs));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, uvs));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, uvs));
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(viewPos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess;
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
    float distance    = length(viewPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, uvs));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, uvs));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, uvs));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main() {
    // norm = vec3(0.0, 1.0, 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, Normal, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < pointLightCount; i++)
        result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);    
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}