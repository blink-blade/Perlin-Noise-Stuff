#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "helpers.h"

extern double** generateNoiseMap(int noiseWidth, int noiseHeight, int layerAmount, double frequency, double octaves, double distanceEffect, int islandMode, double distanceCutOffOffsetX, double distanceCutOffOffsetY, double offset, int landOnly, int distanceCutOff, unsigned int newSeed);