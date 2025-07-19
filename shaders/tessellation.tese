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
struct corner {
    float x;
    float y;
    vec2 gradientVec;
};

float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

vec2 randomGradient(float ix, float iy) {
    vec2 v;
    v.x = sin(ix * rand(vec2(ix, iy)));
    v.y = cos(iy * rand(vec2(ix, iy)));
    return v;
}

// Sample Perlin noise at coordinates x, y
float noise(float x, float y) {
    // Get the corner positions, x0 is left, x1 is left
    float x0 = floor(x); 
    float y0 = floor(y);
    float x1 = x0 + 1.0;
    float y1 = y0 + 1.0;

    // Compute Interpolation weights
    float sx = x - x0;
    float sy = y - y0;

    corner tlCorner;
    corner blCorner;
    corner trCorner;
    corner brCorner;
    tlCorner.x = x0, tlCorner.y = y0;
    blCorner.x = x0, blCorner.y = y1;
    trCorner.x = x1, trCorner.y = y0;
    brCorner.x = x1, brCorner.y = y1;
    tlCorner.gradientVec = randomGradient(tlCorner.x, tlCorner.y);
    blCorner.gradientVec = randomGradient(blCorner.x, blCorner.y);
    trCorner.gradientVec = randomGradient(trCorner.x, trCorner.y);
    brCorner.gradientVec = randomGradient(brCorner.x, brCorner.y);

    // Get distance vectors(A vector from the corner which points to the tile) for each corner of the octant.
    vec2 tlDistanceVector;
    vec2 blDistanceVector;
    vec2 trDistanceVector;
    vec2 brDistanceVector;
    tlDistanceVector.x = x - tlCorner.x, tlDistanceVector.y = y - tlCorner.y; 
    blDistanceVector.x = x - blCorner.x, blDistanceVector.y = y - blCorner.y; 
    trDistanceVector.x = x - trCorner.x, trDistanceVector.y = y - trCorner.y; 
    brDistanceVector.x = x - brCorner.x, brDistanceVector.y = y - brCorner.y; 

    // Get dot product from the distance vectors and the gradient vectors.
    float tlDotProduct = (tlDistanceVector.x * tlCorner.gradientVec.x) + (tlDistanceVector.y * tlCorner.gradientVec.y);
    float blDotProduct = (blDistanceVector.x * blCorner.gradientVec.x) + (blDistanceVector.y * blCorner.gradientVec.y);
    float trDotProduct = (trDistanceVector.x * trCorner.gradientVec.x) + (trDistanceVector.y * trCorner.gradientVec.y);
    float brDotProduct = (brDistanceVector.x * brCorner.gradientVec.x) + (brDistanceVector.y * brCorner.gradientVec.y);

    // Interpolation.
    float tlTrInterpolation = interpolate(tlDotProduct, trDotProduct, sx);
    float blBrInterpolation = interpolate(blDotProduct, brDotProduct, sx);

    return interpolate(tlTrInterpolation, blBrInterpolation, sy);
}

float layeredNoise(float x, float y, int layerAmount, float frequency) {
    // noiseMap[y][x] = -x + y + 10;
    float amp = 1;
    float val = 0;
    float freq = frequency / 50.0;
    for (int i = 0; i < layerAmount; i++) {
        val += noise(x * freq, y * freq) * amp;
        freq *= 2;
        amp /= 2;
    }
   return val;
}

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
    // (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
    vec4 pos = u * pos0 + v * pos1 + w * pos2;

    type = typeE[0];
    // gl_Position = projection * view * vec4(FragPos.x, FragPos.y + layeredNoise(aPos.x, aPos.z, 4, 0.5) * 25, FragPos.z, 1.0);
    FragPos = vec3(pos.x, layeredNoise(pos.x, pos.z, 4, 0.5) * 25, pos.z);
    if (type == 0.0) {
        FragPos = vec3(vec4(pos.x, pos.y, pos.z, 1.0));
    }
    
    
    gl_Position = projection * view * vec4(FragPos.x, FragPos.y, FragPos.z, 1.0);
    Normal = calculateSurfaceNormal(pos0.xyz, pos1.xyz, pos2.xyz);
    uvs = uvsCoord[0];
}
