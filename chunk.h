#ifndef CHUNK_H
#define CHUNK_H

#include "glad.h" 
#include "glm/gtc/quaternion.hpp"
#include "helpers.h" 
#include "generator.h" 
#include "glm/detail/type_mat.hpp"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "glm/gtc/type_ptr.hpp"
#include "shader.h"

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
    Chunk(const glm::vec2 xz, ComputeShader generator, Shader renderShader) {
        pos = glm::vec2(xz.x * chunkSize, xz.y * chunkSize);
        chunkPos = xz;
        generateHeightMap(generator);
        renderShader.use();
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
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
    // -------------
    int xyToI(int x, int y, int width) {
        return x + width*y;
    }
    // ------------------------------------------------------------------------
    void generateHeightMap(ComputeShader generator) {
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
        // heightMap = generateNoiseMap(chunkSize + 2, chunkSize + 2, 4, 0.5, 6, 1, pos.x, pos.y, 0, 0, -0.5, 0, 0, 123);
        // cout << heightMap.x.size();
        float timeVal = glfwGetTime();
        generator.use();
        useTime += glfwGetTime() - timeVal;
        timeVal = glfwGetTime();
        generator.setVec2("pos", pos.x, pos.y);
        setUniformTime += glfwGetTime() - timeVal;
        timeVal = glfwGetTime();
        generator.dispatch();
        dispatchTime += glfwGetTime() - timeVal;
        timeVal = glfwGetTime();
        // Note, I don't care if I don't need to send the entire RGBA values right now. If it needs to be faster, I'll just send the R values, but idc. 
        std::vector<float> imageData(generator.workGroupAmount.x * generator.workGroupAmount.y);
        glBindTexture(GL_TEXTURE_2D, generator.textureID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, imageData.data());
        glBindTexture(GL_TEXTURE_2D, 0);
        transferTime += glfwGetTime() - timeVal;
        timeVal = glfwGetTime();
        float val;
        for (int y = 0; y < chunkSize + 1; y++) {
            for (int x = 0; x < chunkSize + 1; x++) {
                val = imageData[xyToI(x, y, generator.workGroupAmount.x)];
                if (val <= -17.25) {
                    type = 0.0;
                    tL = 0.0f;
                    tB = 0.0f;
                    tR = 0.015625f;
                    tT = 0.015625f;
                }
                else if (val < -13.75) {
                    type = 1.0;
                    tL = 0.015625f;
                    tB = 0.0;
                    tR = 0.03125f;
                    tT = 0.015625f;
                }
                else {
                    type = 2.0;
                    tL = 0.03125f;
                    tB = 0.0f;
                    tR = 0.046875f;
                    tT = 0.015625f;
                }
                // tR -= 0.001;
                // tT -= 0.001;
                // normal = normals[y][x];
                setVertex(x + pos.x, 0.0, y + pos.y, tL, tB, type);
            }   
        }
        verticesTime += glfwGetTime() - timeVal;
        timeVal = glfwGetTime();
        for (int y = 0; y < chunkSize; y++) {
            for (int x = 0; x < chunkSize; x++) {
                indices.push_back(xyToI(x, y, chunkSize + 1));
                indices.push_back(xyToI(x + 1, y, chunkSize + 1));
                indices.push_back(xyToI(x + 1, y + 1, chunkSize + 1));
                indices.push_back(xyToI(x, y, chunkSize + 1));
                indices.push_back(xyToI(x + 1, y + 1, chunkSize + 1));
                indices.push_back(xyToI(x, y + 1, chunkSize + 1));
            }   
        }
        indicesTime += glfwGetTime() - timeVal;
    }
    // ------------------------------------------------------------------------
};

#endif