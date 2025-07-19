#version 460 core

layout (vertices = 3) out;

in vec2 uvs[];
out vec2 uvsCoord[];
 
in float typeC[];
out float typeE[];

uniform vec3 viewPos;
uniform vec2 offset;

void main() {
    vec4 pos = vec4(gl_in[gl_InvocationID].gl_Position.x + offset.x, gl_in[gl_InvocationID].gl_Position.y, gl_in[gl_InvocationID].gl_Position.z + offset.y, gl_in[gl_InvocationID].gl_Position.w);
    gl_out[gl_InvocationID].gl_Position = pos;
    uvsCoord[gl_InvocationID] = uvs[gl_InvocationID];
    typeE[gl_InvocationID] = typeC[gl_InvocationID];
    typeE[gl_InvocationID] = 1.0;
    if (pos.x < 50) {
        typeE[gl_InvocationID] = 0.0;
    }
    float dist = length(viewPos.xyz - pos.xyz);
    // if (dist < 150) {
    //     float num = min(40 / dist, 8);
    //     gl_TessLevelOuter[0] = num; 
    //     gl_TessLevelOuter[1] = num;
    //     gl_TessLevelOuter[2] = num;
    //     num = min(80 / dist, 16);
    //     gl_TessLevelInner[0] = num;
    // }
    // else {
    //     gl_TessLevelOuter[0] = 1;
    //     gl_TessLevelOuter[1] = 1;
    //     gl_TessLevelOuter[2] = 1;
    //     gl_TessLevelInner[0] = 1;
    // }
    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = 1;
    gl_TessLevelOuter[2] = 1;
    gl_TessLevelInner[0] = 0;
}