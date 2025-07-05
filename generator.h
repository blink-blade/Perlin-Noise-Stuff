#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "helpers.h"
#include <vector>

extern vector<vector<float>> generateNoiseMap(int noiseWidth, int noiseHeight, int layerAmount, float frequency, float distanceEffect, int islandMode, int noiseX, int noiseY, float distanceCutOffOffsetX, float distanceCutOffOffsetY, float offset, int landOnly, int distanceCutOff, unsigned int newSeed);