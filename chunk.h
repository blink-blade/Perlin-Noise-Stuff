#ifndef CHUNK_H
#define CHUNK_H

#include "glad.h" 
#include "helpers.h" 
#include "generator.h" 
#include "glm/detail/type_mat.hpp"
#include "glm/glm.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "glm/gtc/type_ptr.hpp"

using namespace std;

class Chunk {
public:
    glm::vec2 chunkPos, pos;
    // x, y, z, tx, ty, type
    vector<float> vertices;
    vector<float> indices;
    vector<glm::vec3> normals;
    vector<glm::vec3> texture;
    vector<vector<float>> heightMap;
    // ------------------------------------------------------------------------
    Chunk(const glm::vec2 xz) {
        pos = glm::vec2(xz.x * chunkSize, xz.y * chunkSize);
        chunkPos = xz;
        generateHeightMap();
    }

    glm::vec3 calculateSurfaceNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        // Need to fix this function, it will only work for half of the triangles. Depending on the orientation of the triangles, the subtractions need done differently.
        glm::vec3 U = p3 - p1;
        glm::vec3 V = p2 - p1;

        glm::vec3 normal((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x));

        return glm::normalize(normal);
    }
    // -------------
    void setVertex(float x, float y, float z, float tX, float tY, int type) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(tX);
        vertices.push_back(tY);
        vertices.push_back(type);
    }
    // ------------------------------------------------------------------------
    void generateHeightMap() {
        int currentIndex, bottomLeftIndex, bottomRightIndex, topLeftIndex, topRightIndex;
        glm::vec3 bottomLeft, bottomRight, topLeft, topRight, normal;
        float tL, tB, tR, tT, type;
        // for (int y = 0; y < chunkSize + 1; y++)
        // {
        //     heightMap.push_back(vector<float>());
        //     for (int x = 0; x < chunkSize + 1; x++)
        //     {
        //         heightMap[y].push_back(x + y);
        //     }
        // }
        heightMap = generateNoiseMap(chunkSize + 1, chunkSize + 1, 4, 1, 6, 1, pos.x, pos.y, 0, 0, 0, 0, 0, 123);
        // cout << heightMap[0].size();
        for (int y = 0; y < chunkSize; y++) {
            for (int x = 0; x < chunkSize; x++) {
                bottomLeft = glm::vec3(x + pos.x, heightMap[y][x], y + pos.y);
                bottomRight = glm::vec3(x + pos.x + 1, heightMap[y][x + 1], y + pos.y);
                topLeft = glm::vec3(x + pos.x, heightMap[y + 1][x], y + pos.y + 1);
                topRight = glm::vec3(x + pos.x + 1, heightMap[y + 1][x + 1], y + pos.y + 1);
                if (heightMap[y][x] < -0.7) {
                    type = 0.0;
                    tL = 0.0f / 1024.0f;
                    tB = 0.0f / 1024.0f;
                    tR = 16.0 / 1024.0f;
                    tT = 16.0 / 1024.0f;
                }
                else if (heightMap[y][x] < -0.55) {
                    type = 1.0;
                    tL = 16.0f / 1024.0f;
                    tB = 0.0f / 1024.0f;
                    tR = 32.0f / 1024.0f;
                    tT = 16.0f / 1024.0f;
                }
                else {
                    type = 2.0;
                    tL = 32.0f / 1024.0f;
                    tB = 0.0f / 1024.0f;
                    tR = 48.0f / 1024.0f;
                    tT = 16.0f / 1024.0f;
                }
                tR -= 0.001;
                tT -= 0.001;
                // tL = (float)x / width;
                // tB = (float)y / height;
                // tR = ((float)x + 1) / width;
                // tT = ((float)y + 1) / height;
                // tL = 0;
                // tB = 0;
                // tR = 1;
                // tT = 1;
                // normal = calculateSurfaceNormal(bottomLeft, topRight, topLeft);
                // normals[x + width*y] = normal;
                setVertex(bottomLeft[0], bottomLeft[1], bottomLeft[2], tL, tB, type);
                setVertex(topRight[0], topRight[1], topRight[2], tR, tT, type);
                setVertex(topLeft[0], topLeft[1], topLeft[2], tL, tT, type);
                // normal = calculateSurfaceNormal(bottomLeft, bottomRight, topRight);
                // normals[x + width*y] = normal;
                setVertex(bottomLeft[0], bottomLeft[1], bottomLeft[2], tL, tB, type);
                setVertex(bottomRight[0], bottomRight[1], bottomRight[2], tR, tB, type);
                setVertex(topRight[0], topRight[1], topRight[2], tR, tT, type);


                normal = calculateSurfaceNormal(bottomLeft, topRight, topLeft);
                normals.push_back(normal);
            }   
        }
    }
    // ------------------------------------------------------------------------
};

#endif