#version 460 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
layout (location = 2) in vec3 aNormal; 

out vec3 ourColor; // output a color to the fragment shader
out vec2 TexCoord;
uniform float time;
uniform mat4 view;
uniform mat4 projection;
out vec3 FragPos;  
out vec3 Normal;

struct corner {
    float x;
    float y;
    vec2 gradientVec;
};

mat4 rotationMatrix(float angleX, float angleY, float angleZ) {
    float cX = cos(angleX);
    float sX = sin(angleX);
    float cY = cos(angleY);
    float sY = sin(angleY);
    float cZ = cos(angleZ);
    float sZ = sin(angleZ);
    return mat4(cZ, sZ, 0.0, 0.0,
                -sZ, cZ, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0) * 
            mat4(cY, 0.0, -sY, 0.0,
                0.0, 1.0, 0.0, 0.0,
                sY, 0.0, cY, 0.0,
                0.0, 0.0, 0.0, 1.0) *
            mat4(1.0, 0.0, 0.0, 0.0,
                0.0, cX, sX, 0.0,
                0.0, -sX, cX, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 randomGradient(float ix, float iy) {
    vec2 v;
    v.x = sin(ix * rand(vec2(ix, iy)));
    v.y = cos(iy * rand(vec2(ix, iy)));
    return v;
}

// Sample Perlin noise at coordinates x, y
float noise(float x, float y, float xOffset, float yOffset) {
    x /= 100; y /= 100;
    // Get the corner positions, x0 is left, x1 is left
    int x0 = int(x); 
    int y0 = int(y);
    int x1 = int(x0 + 1);
    int y1 = int(y0 + 1);

    // Compute Interpolation weights
    float sx = x - x0;
    float sy = y - y0;

    corner tlCorner;
    tlCorner.x = x0; tlCorner.y = y0; tlCorner.gradientVec = randomGradient(x0 + xOffset, y0 + yOffset);
    corner blCorner;
    blCorner.x = x0; blCorner.y = y1; blCorner.gradientVec = randomGradient(x0 + xOffset, y1 + yOffset);
    corner trCorner;
    trCorner.x = x1; trCorner.y = y0; trCorner.gradientVec = randomGradient(x1 + xOffset, y0 + yOffset);
    corner brCorner;
    brCorner.x = x1; brCorner.y = y1; brCorner.gradientVec = randomGradient(x1 + xOffset, y1 + yOffset);

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

float layeredNoise(float x, float y, int layerAmount, float frequency, float gradientXOffset, float gradientYOffset) {
    // noiseMap[y][x] = -x + y + 10;
    // printf("%f\n", noiseMap[y][x]);
    // continue;
    float amp = 1;
    float val = 0;
    float freq = frequency;

    for (int i = 0; i < layerAmount; i++) {
        val += noise(x * freq, y * freq, gradientXOffset, gradientYOffset) * amp;

        freq *= 2;
        amp /= 2;
    }
    
    //if (islandMode == 1) {
        // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
        //distance_to_center = sqrt(pow((x - noiseWidth / 2), 2) + pow((y - noiseHeight / 2), 2));
      //  val = fabs(val) - (distance_to_center / divide_amount);
    //}
    
   return val;
}

void main()
{
    FragPos = vec3(vec4(aPos, 1.0));
    mat4 translation = mat4(1.0, 0.0, 0.0, -1000.0f,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, -1000.0f,
                0.0, 0.0, 0.0, 1.0);
    mat4 rotMatrix = rotationMatrix(0.0f, 0.0f, 0.0f);
    //gl_Position = projection * view * (vec4(aPos.x + (sin(time + (aPos.z / 25)) * 25), aPos.y + (cos(time + (aPos.y / 25)) * 25), aPos.z + (cos(time + (aPos.x / 25)) * 25), 1.0) * translation * rotMatrix);
    //gl_Position = projection * view * (vec4(aPos.x, aPos.y, aPos.z, 1.0));
    //gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    //ourColor = vec3(aColor.x, aColor.y + (cos(time + (aPos.y / 25)) * 5), aColor.z + (sin(time + (aPos.z / 25))));
    //TexCoord = aTexCoord;
    float noiseVal = layeredNoise(aPos.x + time * 100, aPos.z + time * 100, 4, 2, sin(time), tan(time));
    if (noiseVal < -0.4) {
        ourColor = vec3(39.0 / 255, 88.0 / 255, 123.0 / 255);
    }
    else if (noiseVal < -0.3) {
        ourColor = vec3(177.0 / 255, 145.0 / 255, 88.0 / 255);
    }
    else {
        ourColor = vec3(93.0 / 255, 92.0 / 255, 45.0 / 255);
    }
    ourColor.x -= noiseVal / 2;
    ourColor.y -= noiseVal / 2;
    ourColor.z -= noiseVal / 2;



gl_Position = projection * view * vec4(FragPos.x, FragPos.y + noiseVal * 25, FragPos.z, 1.0);
    // Need to fix this later, but if there were a non-uniform scale transform on the vertices, then the normals would need fixed. This is covered in the second page of the lighting section in learnopengl.com
    Normal = aNormal;
}