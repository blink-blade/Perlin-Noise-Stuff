#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "helpers.h"


struct vec2d {
    double x;
    double y;
};

struct corner {
    double x;
    double y;
    struct vec2d gradientVec;
};



unsigned int seed = 100;
double **mapNoiseMap;
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
 
double interpolate(double a0, double a1, double w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}
 
// Sample Perlin noise at coordinates x, y
double noise(double x, double y) {

    // Get the corner positions, x0 is left, x1 is left
    int x0 = (int)x; 
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    double sx = x - (double)x0;
    double sy = y - (double)y0;
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
    double tlDotProduct = (tlDistanceVector.x * tlCorner.gradientVec.x) + (tlDistanceVector.y * tlCorner.gradientVec.y);
    double blDotProduct = (blDistanceVector.x * blCorner.gradientVec.x) + (blDistanceVector.y * blCorner.gradientVec.y);
    double trDotProduct = (trDistanceVector.x * trCorner.gradientVec.x) + (trDistanceVector.y * trCorner.gradientVec.y);
    double brDotProduct = (brDistanceVector.x * brCorner.gradientVec.x) + (brDistanceVector.y * brCorner.gradientVec.y);

    // Interpolation.
    double tlTrInterpolation = interpolate(tlDotProduct, trDotProduct, sx);
    double blBrInterpolation = interpolate(blDotProduct, brDotProduct, sx);
    
    return interpolate(tlTrInterpolation, blBrInterpolation, sy);
}

int getMaxCornerPos(int noiseWidth, int noiseHeight, int layerAmount, double frequency) {
    // The max x or y value that a corner could be positioned at.
    double freq = frequency;
    int maxW = noiseWidth - 1;
    int maxPos;
    for (int ii = 0; ii < layerAmount; ii++) {
        maxPos = (int)((double)maxW * freq);
        freq *= 2;
    }
    return maxPos + 2;
}

void makeCorners(int noiseWidth, int noiseHeight, int layerAmount, double frequency) {
    int max = getMaxCornerPos(noiseWidth, noiseHeight, layerAmount, frequency);
    struct corner corner;

    // Allocate memory for the array.
    cornerGrid = new struct corner*[max];

    for (int h = 0; h < max; h++) {
        cornerGrid[h] = new struct corner[max];
    }

    for (int x = 0; x < max; x++) {
        for (int y = 0; y < max; y++) {
            corner.x = x;
            corner.y = y;
            corner.gradientVec = randomGradient(x, y);
            cornerGrid[y][x] = corner;
        }
    }
}

double** generateNoiseMap(int noiseWidth, int noiseHeight, int layerAmount, double frequency, double distanceEffect, int islandMode, double distanceCutOffOffsetX, double distanceCutOffOffsetY, double offset, int landOnly, int distanceCutOff, unsigned int newSeed) {
    int x, y, i;
    double cutOffCenterPointX = ((double)noiseWidth / 2) + distanceCutOffOffsetX;
    double cutOffCenterPointY = ((double)noiseHeight / 2) + distanceCutOffOffsetY;
    double val = 0, freq;
    double divide_amount = 0;
    double divide_amountX = 0;
    double divide_amountY = 0;
    if (distanceEffect) {
        divide_amount = ((double)(noiseWidth / distanceEffect) + (double)(noiseHeight / distanceEffect));
    }
    else {
        divide_amount = ((double)(noiseWidth) + (double)(noiseHeight));
    }
    
    double distance_to_center;
    unsigned int * pSeed = &seed;
    *pSeed = newSeed;

    // Allocate memory for the array.
    double** noiseMap = 0;
    noiseMap = new double*[noiseHeight];

    for (int h = 0; h < noiseHeight; h++)
    {
        noiseMap[h] = new double[noiseWidth];
    }

    frequency /= 50;
    
    makeCorners(noiseWidth, noiseHeight, layerAmount, frequency);

    for (int x = 0; x < noiseWidth; x++)
    {
        for (int y = 0; y < noiseHeight; y++)
        {
            // noiseMap[y][x] = -x + y + 10;
            // printf("%f\n", noiseMap[y][x]);
            // continue;
            double amp = 1;
            val = 0;
            freq = frequency;
            for (int i = 0; i < layerAmount; i++) {
                val += noise((double)x * freq, (double)y * freq) * amp;

                freq *= 2;
                amp /= 2;
            }
            
            if (islandMode == 1) {
                // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
                distance_to_center = sqrt(pow((x - noiseWidth / 2), 2) + pow((y - noiseHeight / 2), 2));
                val = fabs(val) - (distance_to_center / divide_amount);
            }
            else {
                // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
                distance_to_center = sqrt(pow((x - noiseWidth / 2), 2) + pow((y - noiseHeight / 2), 2));
                val -= (distance_to_center / divide_amount);
            }
            
            val += offset;

            noiseMap[y][x] = val;
        }
    }
    return noiseMap;
}


extern void generateMap(int layerAmount, double frequency, unsigned int mapSeed) {
    int x, y, sameTypeAdjacents, adjacentType;
    double val = 0;
    
    mapNoiseMap = generateNoiseMap(width, height, layerAmount, frequency, 6, 1, 0, 0, 0, 0, 0, mapSeed);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            val = mapNoiseMap[y][x];

            mapNoiseMap[y][x] = val;
            // Set the the tile type depending on the value.
        }
    }
}

// This is for getting the noise values with python.
double getNoiseValue(int x, int y) {
    return mapNoiseMap[y][x];
}