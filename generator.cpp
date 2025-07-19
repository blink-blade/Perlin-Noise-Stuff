#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "helpers.h"
#include <vector>
#include <algorithm>
#include <iostream>

struct vec2d {
    float x;
    float y;
};

struct corner {
    float x;
    float y;
    glm::vec2 gradientVec;
};


unsigned int seed = 100;
float **mapNoiseMap;
struct corner **cornerGrid;

struct vec2d possibleGradientVectors[8];

float randd(glm::vec2 co) {
    return glm::fract(sin(glm::dot(co, glm::vec2(12.9898,78.233))) * 43758.5453);
}

glm::vec2 randomGradient(float ix, float iy) {
    glm::vec2 v;
    v.x = sin(ix * randd(glm::vec2(ix, iy)));
    v.y = cos(iy * randd(glm::vec2(ix, iy)));
    return v;
}

 
float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}
 
// Sample Perlin noise at coordinates x, y
float noise(float x, float y) {
    // Get the corner positions, x0 is left, x1 is left
    int x0 = (int)x; 
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    struct corner tlCorner;
    struct corner blCorner;
    struct corner trCorner;
    struct corner brCorner;
    tlCorner.x = x0, tlCorner.y = y0;
    blCorner.x = x0, blCorner.y = y1;
    trCorner.x = x1, trCorner.y = y0;
    brCorner.x = x1, brCorner.y = y1;
    tlCorner.gradientVec = randomGradient(tlCorner.x, tlCorner.y);
    blCorner.gradientVec = randomGradient(blCorner.x, blCorner.y);
    trCorner.gradientVec = randomGradient(trCorner.x, trCorner.y);
    brCorner.gradientVec = randomGradient(brCorner.x, brCorner.y);

    // Get distance vectors(A vector from the corner which points to the tile) for each corner of the octant.
    struct vec2d tlDistanceVector;
    struct vec2d blDistanceVector;
    struct vec2d trDistanceVector;
    struct vec2d brDistanceVector;
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

vector<vector<float>> generateNoiseMap(int noiseWidth, int noiseHeight, int layerAmount, float frequency, float distanceEffect, int islandMode, int noiseX, int noiseY, float distanceCutOffOffsetX, float distanceCutOffOffsetY, float offset, int landOnly, int distanceCutOff, unsigned int newSeed) {
    int x, y, i;
    float cutOffCenterPointX = ((float)noiseWidth / 2) + distanceCutOffOffsetX;
    float cutOffCenterPointY = ((float)noiseHeight / 2) + distanceCutOffOffsetY;
    float val = 0, freq;
    float divide_amount = 0;
    float divide_amountX = 0;
    float divide_amountY = 0;
    if (distanceEffect) {
        divide_amount = ((float)(width / distanceEffect) + (float)(width / distanceEffect));
    }
    else {
        divide_amount = ((float)(width) + (float)(width));
    }
    
    float distance_to_center;
    unsigned int * pSeed = &seed;
    *pSeed = newSeed;

    // Allocate memory for the array.
    vector<vector<float>> noiseMap;

    frequency /= 50.0;
    
    // makeCorners(noiseWidth, noiseHeight, noiseX, noiseY, layerAmount, frequency);
    for (int y = noiseY; y < noiseHeight + noiseY; y++)
    {
        noiseMap.push_back(vector<float>());
        for (int x = noiseX; x < noiseWidth + noiseX; x++)
        {
            // cout << x << " " << y << endl;
            // noiseMap[y][x] = -x + y + 10;
            // printf("%f\n", noiseMap[y][x]);
            // continue;
            float amp = 1;
            val = 0;
            freq = frequency;
            for (int i = 0; i < layerAmount; i++) {
                val += noise((float)x * freq, (float)y * freq) * amp;

                freq *= 2;
                amp /= 2;
            }
            
            // if (islandMode == 1) {
            //     // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
            //     distance_to_center = sqrt(pow((x - width / 2), 2) + pow((y - height / 2), 2));
            //     val = fabs(val) - (distance_to_center / divide_amount);
            // }
            // else {
            //     // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
            //     distance_to_center = sqrt(pow((x - width / 2), 2) + pow((y - height / 2), 2));
            //     val -= (distance_to_center / divide_amount);
            // }
            
            // val += offset;

            noiseMap[y - noiseY].push_back(val * 25);
        }
    }
    return noiseMap;
}


// extern void generateMap(int layerAmount, double frequency, unsigned int mapSeed) {
//     int x, y, sameTypeAdjacents, adjacentType;
//     double val = 0;
    
//     mapNoiseMap = generateNoiseMap(width, height, layerAmount, frequency, 6, 1, 0, 0, 0, 0, 0, mapSeed);

//     for (int x = 0; x < width; x++) {
//         for (int y = 0; y < height; y++) {
//             val = mapNoiseMap[y][x];

//             mapNoiseMap[y][x] = val;
//             // Set the the tile type depending on the value.
//         }
//     }
// }

// This is for getting the noise values with python.
float getNoiseValue(int x, int y) {
    return mapNoiseMap[y][x];
}