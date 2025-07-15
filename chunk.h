#ifndef CHUNK_H
#define CHUNK_H

#include "glad.h" 
#include "glm/gtc/quaternion.hpp"
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
    vector<unsigned int> indices;
    vector<glm::vec3> texture;
    vector<vector<float>> heightMap;
    vector<vector<glm::vec3>> normals;
    unsigned int VAO, VBO, EBO;

    // ------------------------------------------------------------------------
    Chunk(const glm::vec2 xz) {
        pos = glm::vec2(xz.x * chunkSize, xz.y * chunkSize);
        chunkPos = xz;
        generateHeightMap();
        // Rendering stuff.
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);
    }

    glm::vec3 calculateSurfaceNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        // Need to fix this function, it will only work for half of the triangles. Depending on the orientation of the triangles, the subtractions need done differently.
        glm::vec3 U = p3 - p1;
        glm::vec3 V = p2 - p1;

        glm::vec3 normal((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x));

        return glm::normalize(normal);
    }
    // -------------
    void setVertex(float x, float y, float z, float nX, float nY, float nZ, float tX, float tY, int type) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(nX);
        vertices.push_back(nY);
        vertices.push_back(nZ);
        vertices.push_back(tX);
        vertices.push_back(tY);
        vertices.push_back(type);
    }
    // -------------
    int xyToI(int x, int y, int width) {
        return x + width*y;
    }
    // ------------------------------------------------------------------------
    void generateHeightMap() {
        int currentIndex, bottomLeftIndex, bottomRightIndex, topLeftIndex, topRightIndex;
        glm::vec3 bottomLeft, bottomRight, topLeft, topRight, normal;
        glm::vec2 bottomLeftXY, bottomRightXY, topLeftXY, topRightXY;
        float tL, tB, tR, tT, type;
        // for (int y = 0; y < chunkSize + 1; y++)
        // {
        //     heightMap.push_back(vector<float>());
        //     for (int x = 0; x < chunkSize + 1; x++)
        //     {
        //         heightMap[y].push_back(x + y);
        //     }
        // }
        heightMap = generateNoiseMap(chunkSize + 2, chunkSize + 2, 4, 0.5, 6, 1, pos.x, pos.y, 0, 0, -0.5, 0, 0, 123);
        // cout << heightMap.x.size();

        for (int y = 0; y < chunkSize + 2; y++) {
            normals.push_back(vector<glm::vec3>());
            for (int x = 0; x < chunkSize + 2; x++) {
                normals[y].push_back(glm::vec3(0.0, 0.0, 0.0));
            }   
        }
        for (int y = 0; y < chunkSize + 1; y++) {
            for (int x = 0; x < chunkSize + 1; x++) {
                bottomLeft = glm::vec3(x, heightMap[y][x], y);
                bottomRight = glm::vec3(x + 1, heightMap[y][x + 1], y);
                topLeft = glm::vec3(x, heightMap[y + 1][x], y + 1);
                topRight = glm::vec3(x + 1, heightMap[y + 1][x + 1], y + 1);
                normal = calculateSurfaceNormal(bottomLeft, topRight, topLeft);
                normals[bottomLeft.z][bottomLeft.x] += normal;
                normals[topRight.z][topRight.x] += normal;
                normals[topLeft.z][topLeft.x] += normal;
                normal = calculateSurfaceNormal(bottomLeft, bottomRight, topRight);
                normals[bottomLeft.z][bottomLeft.x] += normal;
                normals[bottomRight.z][bottomRight.x] += normal;
                normals[topRight.z][topRight.x] += normal;
            }   
        }
        for (int y = 0; y < chunkSize + 1; y++) {
            for (int x = 0; x < chunkSize + 1; x++) {
                normals[y][x] = glm::normalize(normals[y][x]);
            }   
        }
        for (int y = 0; y < chunkSize + 1; y++) {
            normals.push_back(vector<glm::vec3>());
            for (int x = 0; x < chunkSize + 1; x++) {
                if (heightMap[y][x] <= -17.25) {
                    type = 0.0;
                    tL = 0.0f / 1024.0f;
                    tB = 0.0f / 1024.0f;
                    tR = 16.0 / 1024.0f;
                    tT = 16.0 / 1024.0f;
                }
                else if (heightMap[y][x] < -13.75) {
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
                normal = normals[y][x];
                setVertex(x + pos.x, heightMap[y][x], y + pos.y, normal.x, normal.y, normal.z, tL, tB, type);
            }   
        }
        for (int y = 0; y < chunkSize; y++) {
            for (int x = 0; x < chunkSize; x++) {
                indices.push_back(xyToI(x, y, chunkSize + 1));
                indices.push_back(xyToI(x + 1, y, chunkSize + 1));
                indices.push_back(xyToI(x + 1, y + 1, chunkSize + 1));
                indices.push_back(xyToI(x, y + 1, chunkSize + 1));
            }   
        }
    }
    // ------------------------------------------------------------------------
};

#endif