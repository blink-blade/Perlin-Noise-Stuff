#include "stdint.h"
#include "glad.h" 
#include "khrplatform.h"
#include <GLFW/glfw3.h>
#include "inttypes.h"
#include <cinttypes>
#include <cstdlib>
#include <ostream>
#include <iostream>
#include "shader.h"
#include "light.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "generator.h"
#include "filesystem.h"

using namespace std;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

const char *vertexShaderSource;
const char *fragmentShaderSource;

// float vertices[] = {
//     // positions          // colors           // texture coords
//      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  
//      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   
//     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  
//     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   
// };
glm::vec3 translations[100];
float vertices[54000000]; //width * height * 9.       * 12 for (x, y, z, r, g, b, ect) and * 6 for six vertices on each face.
float cubeVertices[216]= {
    -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 

    -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f, 

    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 

        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f, -0.5f, 

    -0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f
};
// float vertices[] = {
//     -0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f
// };
double **noiseMap;

int setVertex(int index, float x, float y, float z, float r, float b, float g, float nVX, float nVY, float nVZ, float tX, float tY, int type) {
    int newIndex = index;
    vertices[newIndex] = x;
    newIndex += 1;
    vertices[newIndex] = y;
    newIndex += 1;
    vertices[newIndex] = z;
    newIndex += 1;
    vertices[newIndex] = r;
    newIndex += 1;
    vertices[newIndex] = g;
    newIndex += 1;
    vertices[newIndex] = b;
    newIndex += 1;
    // vertices[newIndex] = nVX;
    // newIndex += 1;
    // vertices[newIndex] = nVY;
    // newIndex += 1;
    // vertices[newIndex] = nVZ;
    // newIndex += 1;
    vertices[newIndex] = tX;
    newIndex += 1;
    vertices[newIndex] = tY;
    newIndex += 1;
    vertices[newIndex] = type;
    return newIndex + 1;
}

glm::vec3 calculateSurfaceNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    // Need to fix this function, it will only work for half of the triangles. Depending on the orientation of the triangles, the subtractions need done differently.
	glm::vec3 U = p3 - p1;
	glm::vec3 V = p2 - p1;

	glm::vec3 normal;
	normal.x = (U.y * V.z) - (U.z * V.y);
	normal.y = (U.z * V.x) - (U.x * V.z);
	normal.z = (U.x * V.y) - (U.y * V.x);

	return glm::normalize(normal);
}

int getIndexOfPoint(int x, int z) {
    return (x + width*z);
}


void makeFaces() {
    int currentIndex, bottomLeftIndex, bottomRightIndex, topLeftIndex, topRightIndex;
    glm::vec3 bottomLeft, bottomRight, topLeft, topRight, pos, normal;
    float r = 1, g = 1, b = 1, tL, tB, tR, tT, type;
    currentIndex = 0;
    // 0.545474
    // 0.0541736
    float timeTakenToStart = glfwGetTime();
    noiseMap = generateNoiseMap(width + 1, width + 1, 8, 0.3, 6, 1, 0, 0, 0, 0, 0, 123);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            bottomLeft = glm::vec3(x, noiseMap[y][x] * 45, y);
            bottomRight = glm::vec3(x + 1, noiseMap[y][x + 1] * 45, y);
            topLeft = glm::vec3(x, noiseMap[y + 1][x] * 45, y + 1);
            topRight = glm::vec3(x + 1, noiseMap[y + 1][x + 1] * 45, y + 1);
            if (noiseMap[y][x] < -0.7) {
                type = 0.0;
                tL = 0.0f / 1024.0f;
                tB = 0.0f / 1024.0f;
                tR = 16.0 / 1024.0f;
                tT = 16.0 / 1024.0f;
                // r = 39.0 / 255;
                // b = 88.0 / 255;
                // g = 123.0 / 255;
            }
            else if (noiseMap[y][x] < -0.55) {
                type = 1.0;
                tL = 16.0f / 1024.0f;
                tB = 0.0f / 1024.0f;
                tR = 32.0f / 1024.0f;
                tT = 16.0f / 1024.0f;
                // r = 177.0 / 255;
                // b = 145.0 / 255;
                // g = 88.0 / 255;
            }
            else {
                type = 2.0;
                tL = 32.0f / 1024.0f;
                tB = 0.0f / 1024.0f;
                tR = 48.0f / 1024.0f;
                tT = 16.0f / 1024.0f;
                // r = 93.0 / 255;
                // b = 92.0 / 255;
                // g = 45.0 / 255;
            }
            tR -= 0.001;
            tT -= 0.001;
            // tL = (float)x / width;
            // tB = (float)y / height;
            // tR = ((float)x + 1) / width;
            // tT = ((float)y + 1) / height;
            normal = calculateSurfaceNormal(bottomLeft, topRight, topLeft);
            currentIndex = setVertex(currentIndex, bottomLeft[0], bottomLeft[1], bottomLeft[2], r, g, b, normal[0], normal[1], normal[2], tL, tB, type);
            currentIndex = setVertex(currentIndex, topRight[0], topRight[1], topRight[2], r, g, b, normal[0], normal[1], normal[2], tR, tT, type);
            currentIndex = setVertex(currentIndex, topLeft[0], topLeft[1], topLeft[2], r, g, b, normal[0], normal[1], normal[2], tL, tT, type);
            normal = calculateSurfaceNormal(bottomLeft, bottomRight, topRight);
            currentIndex = setVertex(currentIndex, bottomLeft[0], bottomLeft[1], bottomLeft[2], r, g, b, normal[0], normal[1], normal[2], tL, tB, type);
            currentIndex = setVertex(currentIndex, bottomRight[0], bottomRight[1], bottomRight[2], r, g, b, normal[0], normal[1], normal[2], tR, tB, type);
            currentIndex = setVertex(currentIndex, topRight[0], topRight[1], topRight[2], r, g, b, normal[0], normal[1], normal[2], tR, tT, type);
        }   
    }
    
    // glm::vec3 av = glm::vec3(-0.5f, -0.5f, -0.5f);
    // glm::vec3 bv = glm::vec3(0.5f, -0.5f, -0.5f);
    // glm::vec3 cv = glm::vec3(0.5f, 0.5f, -0.5f);

    // cout << [0] << ", " << calculateSurfaceNormal(av, bv, cv)[1] << ", " << calculateSurfaceNormal(av, bv, cv)[2] << "\n";

    // currentIndex = setVertex(0, -0.5f, -0.5f, -0.5f, r, g, b, 0, 0, 0);
    // currentIndex = setVertex(currentIndex, 0.5f, -0.5f, -0.5f, r, g, b, 0, 0, 0);
    // currentIndex = setVertex(currentIndex, 0.5f,  0.5f, -0.5f, r, g, b, 0, 0, 0);

    
    cout << "Time was: " << glfwGetTime() - timeTakenToStart << "\n";
}

int main()
{
    glfwInits();
    Shader lightingShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Shader lightCubeShader("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");
    makeFaces();
    lightingShader.use(); // don't forget to activate/use the shader before setting uniforms!

    // Rendering stuff.
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glUniform1i(glGetUniformLocation(lightingShader.ID, "texture1"), 0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // first, configure the cube's VAO (and VBO)
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); 

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;
Light lights[1000];
// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("heightTexture", 2);
    lightingShader.setInt("width", width);
    lightingShader.setInt("height", height);
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("images/spritesheet.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("images/spritesheetspecularmap.png").c_str());
    srand(time(0));
    Light light;
    // for (int i = 0; i < 1000; i++) {
    //     float r = round(sin(rand()) * 0.5 + 0.5);
    //     float b = round(sin(rand()) * 0.5 + 0.5);
    //     float g = round(sin(rand()) * 0.5 + 0.5);
    //     light = Light(2.0, glm::vec3(sin(rand()) * 400, 30, sin(rand()) * 400), glm::vec3(r / 20, g / 20, b / 20), glm::vec3(r / 1.25, g / 1.25, b / 1.25), glm::vec3(r, g, b), 25, lightingShader);
    //     light.init(lightingShader);
    //     lights[light.ID] = light;
    // }

    const char* text = to_string(1000 / deltaTime / 1000).c_str();
    int jeffy = 0;
    float deltaTimeList[50];
    for (int x = 0; x < 10; x++) {
        for (int z = 0; z < 10; z++) {
            translations[x + 10*z].x = x * 1000;
            translations[x + 10*z].z = z * 1000;
        }
    }
    for(unsigned int i = 0; i < 100; i++) {
        lightingShader.setVec3(("offsets[" + std::to_string(i) + "]"), translations[i].x, translations[i].y, translations[i].z);
    }  

    vector<float> flatData(width * height);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            flatData[y * width + x] = noiseMap[y][x];  // or float**[y][x]
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload the float data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, flatData.data());

    // Set texture filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 18 fps when looking down at the entire map with half the screen. (I go up until I can barely see the entire map)
    while(!glfwWindowShouldClose(window))   {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        // cout << "\n" << deltaTime;
        deltaTimeList[jeffy] = deltaTime;
        jeffy += 1;
        // cout << jeffy << " " << jeffy % 50 << "\n";
        if (jeffy % 50 == 0) {
            jeffy = 0;
            float total = 0;
            for (int i = 0; i < 50; i++) {
                total += deltaTimeList[i];
            }
            text = to_string(1000 / (total / 50) / 1000).c_str();
            glfwSetWindowTitle(window, text);
        }
        // cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << "\n";
        cameraSpeed = 500.5f * deltaTime;
        processInput(window);
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glm::mat4 trans = glm::mat4(1.0f);
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0, 1.0, 0.0));
        // trans = glm::scale(trans, glm::vec3(sin((float)glfwGetTime()), 1.0, 1.0));  

        // unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        // update the uniform color
        // float timeValue = glfwGetTime();
        // float greenValue = sin(timeValue);
        // ourShader.use();
        // int vertexColorLocation = glGetUniformLocation(ourShader.ID, "timeOffsetColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        // ourShader.setFloat("mixAmount", sin(timeValue));
        float timeValue = glfwGetTime();

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        // bind height map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureID);


        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 10000.0f);


        lightingShader.use();
        lightingShader.setVec3("viewPos", cameraPos[0], cameraPos[1], cameraPos[2]); 
        lightingShader.setFloat("material.shininess", 1.0f);
        lightingShader.setVec3("dirLight.direction", -0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.05f * 2, 0.05f * 2, 0.05f * 2);
        lightingShader.setVec3("dirLight.diffuse", 0.4f * 2, 0.4f * 2, 0.4f * 2);
        lightingShader.setVec3("dirLight.specular", 0.5f * 2, 0.5f * 2, 0.5f * 2);

        float greenValue = sin(timeValue);
        lightingShader.setFloat("timeOffsetColor", sin(timeValue));
        int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");
        glUniformMatrix4fv( projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        int timeLoc = glGetUniformLocation(lightingShader.ID, "time");
        glUniform1f(timeLoc, (float)glfwGetTime());


        // draw the object
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 72000000 / 6);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 54000000 / 9, 1); 

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(cubeVAO);
        for (int i = 0; i < pointLightCount; i++) {
            lights[i].render(lightCubeShader);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);     
    glfwTerminate();

    return 0;
}