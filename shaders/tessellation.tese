#version 460 core

layout (triangles, equal_spacing, ccw) in;

in vec2 uvsCoord[];
out vec2 uvs;

in float typeE[];
out vec3 FragPos; 
out vec3 Normal;
out float type;

uniform float time;
uniform mat4 view;
uniform mat4 projection;
#define PI 3.1415926535897932384626433832795

vec3 calculateSurfaceNormal(vec3 p1, vec3 p2, vec3 p3) {
    // Need to fix this function, it will only work for half of the triangles. Depending on the orientation of the triangles, the subtractions need done differently.
    vec3 U = p3 - p1;
    vec3 V = p2 - p1;

    vec3 normal = vec3((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x));

    return normalize(normal);
}

void main() {
    // barycentric coordinates
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    // barycentric interpolation
    vec2 texCoord = u * uvsCoord[0] + v * uvsCoord[1] + w * uvsCoord[2];

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    // barycentric interpolation
    vec4 pos = u * pos0 + v * pos1 + w * pos2;

    type = typeE[0];
    // gl_Position = projection * view * vec4(FragPos.x, FragPos.y + layeredNoise(aPos.x, aPos.z, 4, 0.5) * 25, FragPos.z, 1.0);
    FragPos = vec3(pos.x, pos.y, pos.z);
    
    
    gl_Position = projection * view * vec4(FragPos.x, FragPos.y, FragPos.z, 1.0);
    Normal = calculateSurfaceNormal(pos0.xyz, pos1.xyz, pos2.xyz);
    uvs = uvsCoord[0];
}
