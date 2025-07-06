#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "helpers.h"
#include <vector>

struct vec2d {
    float x;
    float y;
};

struct corner {
    float x;
    float y;
    struct vec2d gradientVec;
};



unsigned int seed = 100;
float **mapNoiseMap;
struct corner **cornerGrid;

struct vec2d possibleGradientVectors[8];

struct vec2d randomGradient(int ix, int iy) {

    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443 * seed;
 
    b ^= a << s | a >> w - s;
    b *= 1911520717 * seed;
 
    a ^= b << s | b >> w - s;
    a *= 2048419325 * seed;
    struct vec2d v;
    v.x = sin(a);
    v.y = cos(a);
    return v;
}
 
float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}
 
// Sample Perlin noise at coordinates x, y
float noise(float x, float y, int noiseX, int noiseY) {
    // cout << "start" << endl;
    // Get the corner positions, x0 is left, x1 is left
    int x0 = (int)x; 
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    x += noiseX;
    y += noiseY;
    // cout << x0 << " " << y0 << endl;
    // cout << sx << ", " << sy << "\n";
    struct corner tlCorner = cornerGrid[y0][x0];
    struct corner blCorner = cornerGrid[y1][x0];
    struct corner trCorner = cornerGrid[y0][x1];
    struct corner brCorner = cornerGrid[y1][x1];

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

int getMaxCornerPos(int noiseWidth, int noiseHeight, int layerAmount, float frequency) {
    // The max x or y value that a corner could be positioned at.
    float freq = frequency;
    int maxW = noiseWidth - 1;
    int maxPos;
    for (int ii = 0; ii < layerAmount; ii++) {
        maxPos = (int)((float)maxW * freq);
        freq *= 2;
    }
    return maxPos + 1;
}

void makeCorners(int noiseWidth, int noiseHeight, int noiseX, int noiseY, int layerAmount, float frequency) {
    int max = getMaxCornerPos(noiseWidth, noiseHeight, layerAmount, frequency);
    struct corner corner;

    delete cornerGrid;
    // Allocate memory for the array.
    cornerGrid = new struct corner*[max];

    for (int h = 0; h < max; h++) {
        cornerGrid[h] = new struct corner[max];
    }
    // cout << max << endl;
    for (int x = 0; x < max; x++) {
        for (int y = 0; y < max; y++) {
            corner.x = x + noiseX;
            corner.y = y + noiseY;
            corner.gradientVec = randomGradient(x + noiseX, y + noiseY);
            cornerGrid[y][x] = corner;
        }
    }
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

    frequency /= 50;
    
    makeCorners(noiseWidth, noiseHeight, noiseX, noiseY, layerAmount, frequency);
    
    for (int y = 0; y < noiseHeight; y++)
    {
        noiseMap.push_back(vector<float>());
        for (int x = 0; x < noiseWidth; x++)
        {
            // cout << x << " " << y << endl;
            // noiseMap[y][x] = -x + y + 10;
            // printf("%f\n", noiseMap[y][x]);
            // continue;
            float amp = 1;
            val = 0;
            freq = frequency;
            for (int i = 0; i < 1; i++) {
                val += noise((float)x * freq, (float)y * freq, noiseX, noiseY) * amp;

                freq *= 2;
                amp /= 2;
            }
            
            // if (islandMode == 1) {
            //     // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
            //     distance_to_center = sqrt(pow((x + noiseX - width / 2), 2) + pow((y + noiseY - height / 2), 2));
            //     val = fabs(val) - (distance_to_center / divide_amount);
            // }
            // else {
            //     // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
            //     distance_to_center = sqrt(pow((x + noiseX - width / 2), 2) + pow((y + noiseY - height / 2), 2));
            //     val -= (distance_to_center / divide_amount);
            // }
            
            // val += offset;

            noiseMap[y].push_back(val);
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