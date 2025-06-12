#include "stdint.h"
#include "glad.h" 
#include "khrplatform.h"
#include <GLFW/glfw3.h>
#include "inttypes.h"
#include <ostream>
#include <iostream>
#include "helpers.h"
#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "generator.h"

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
float vertices[48000000]; //width * height * 6 * 6.       * 6 for (x, y, z, r, g, b) and * 6 for six vertices on each face.
// float vertices[] = {
//     -0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f
// };
double **noiseMap;

int setVertex(int index, float x, float y, float z, float r, float b, float g, float tX, float tY) {
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
    vertices[newIndex] = tX;
    newIndex += 1;
    vertices[newIndex] = tY;
    return newIndex + 1;
}


void makeFaces() {
    int faceIndex, currentIndex, firstTriangleIndex, secondTriangleIndex;
    int bottomLeft[2], bottomRight[2], topLeft[2], topRight[2];
    float r = 1, g = 1, b = 1;
    currentIndex = 0;
    // 0.545474
    // 0.0541736
    float timeTakenToStart = glfwGetTime();
    noiseMap = generateNoiseMap(width + 1, width + 1, 5, 2, 6, 1, 0, 0, 0, 0, 0, 123);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // I believe we need to go in a counter-clockwise order of vertices.
            bottomLeft[0] = x; bottomLeft[1] = y;
            bottomRight[0] = x + 1; bottomRight[1] = y;
            topLeft[0] = x; topLeft[1] = y + 1;
            topRight[0] = x + 1; topRight[1] = y + 1;
            r = 0.1, g = 0.1, b = 0.1;
            if (noiseMap[y][x] < -0.7) {
                r = 39.0 / 255;
                b = 88.0 / 255;
                g = 123.0 / 255;
            }
            else if (noiseMap[y][x] < -0.3) {
                r = 177.0 / 255;
                b = 145.0 / 255;
                g = 88.0 / 255;
            }
            else {
                r = 93.0 / 255;
                b = 92.0 / 255;
                g = 45.0 / 255;
            }
            r -= noiseMap[y][x];
            b -= noiseMap[y][x];
            g -= noiseMap[y][x];
            currentIndex = setVertex(currentIndex, bottomLeft[0], noiseMap[bottomLeft[1]][bottomLeft[0]] * 45, bottomLeft[1], r, g, b, 0.0, 0.0);
            currentIndex = setVertex(currentIndex, topRight[0], noiseMap[topRight[1]][topRight[0]] * 45, topRight[1], r, g, b, 1.0, 1.0);
            currentIndex = setVertex(currentIndex, topLeft[0], noiseMap[topLeft[1]][topLeft[0]] * 45, topLeft[1], r, g, b, 0.0, 1.0);
            currentIndex = setVertex(currentIndex, bottomLeft[0], noiseMap[bottomLeft[1]][bottomLeft[0]] * 45, bottomLeft[1], r, g, b, 0.0, 0.0);
            currentIndex = setVertex(currentIndex, topRight[0], noiseMap[topRight[1]][topRight[0]] * 45, topRight[1], r, g, b, 1.0, 1.0);
            currentIndex = setVertex(currentIndex, bottomRight[0], noiseMap[bottomRight[1]][bottomRight[0]] * 45, bottomRight[1], r, g, b, 1.0, 0.0);
        }   
    }
    cout << "Time was: " << glfwGetTime() - timeTakenToStart << "\n";
}

int main()
{
    glfwInits();
    Shader cubeShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    makeFaces();

    cubeShader.use(); // don't forget to activate/use the shader before setting uniforms!

    // Rendering stuff.
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("images/john.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glUniform1i(glGetUniformLocation(cubeShader.ID, "texture1"), 0);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;

// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!glfwWindowShouldClose(window))   {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        cameraSpeed = 100.5f * deltaTime;
        processInput(window);
        scroll = 0;
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 10000.0f);


        cubeShader.use();
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue);
        cubeShader.setFloat("timeOffsetColor", sin(timeValue));
        int viewLoc = glGetUniformLocation(cubeShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(cubeShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        int timeLoc = glGetUniformLocation(cubeShader.ID, "time");
        glUniform1f(timeLoc, (float)glfwGetTime());

        // draw the object
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 48000000); 
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