#version 460 core

layout (vertices = 3) out;

in vec2 uvs[];
out vec2 uvsCoord[];
 
in float typeC[];
out float typeE[];

uniform vec3 viewPos;

void main() {
    vec4 pos = vec4(gl_in[gl_InvocationID].gl_Position.x, gl_in[gl_InvocationID].gl_Position.y, gl_in[gl_InvocationID].gl_Position.z, gl_in[gl_InvocationID].gl_Position.w);
    gl_out[gl_InvocationID].gl_Position = pos;
    uvsCoord[gl_InvocationID] = uvs[gl_InvocationID];
    typeE[gl_InvocationID] = typeC[gl_InvocationID];
    float dist = length(viewPos.xyz - pos.xyz);
    if (dist < 150) {
        gl_TessLevelOuter[0] = 40 / dist; 
        gl_TessLevelOuter[1] = 40 / dist;
        gl_TessLevelOuter[2] = 40 / dist;
        gl_TessLevelInner[0] = 80 / dist;
    }
    else {
        gl_TessLevelOuter[0] = 1;
        gl_TessLevelOuter[1] = 1;
        gl_TessLevelOuter[2] = 1;
        gl_TessLevelInner[0] = 1;
    }
}