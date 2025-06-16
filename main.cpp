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
float vertices[66000000]; //width * height * 6 * 6.       * 6 for (x, y, z, r, g, b) and * 6 for six vertices on each face.
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

int setVertex(int index, float x, float y, float z, float r, float b, float g, float nVX, float nVY, float nVZ, float tX, float tY) {
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
    vertices[newIndex] = nVX;
    newIndex += 1;
    vertices[newIndex] = nVY;
    newIndex += 1;
    vertices[newIndex] = nVZ;
    newIndex += 1;
    vertices[newIndex] = tX;
    newIndex += 1;
    vertices[newIndex] = tY;
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

void makeFaces() {
    int faceIndex, currentIndex, firstTriangleIndex, secondTriangleIndex;
    glm::vec3 bottomLeft, bottomRight, topLeft, topRight, normal;
    float r = 1, g = 1, b = 1;
    currentIndex = 0;
    // 0.545474
    // 0.0541736
    float timeTakenToStart = glfwGetTime();
    noiseMap = generateNoiseMap(width + 1, width + 1, 4, 2, 6, 1, 0, 0, 0, 0, 0, 123);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // I believe we need to go in a counter-clockwise order of vertices.
            bottomLeft = glm::vec3(x - 500, noiseMap[y][x] * 45, y - 500);
            bottomRight = glm::vec3(x - 500 + 1, noiseMap[y][x + 1] * 45, y - 500);
            topLeft = glm::vec3(x - 500, noiseMap[y + 1][x] * 45, y - 500 + 1);
            topRight = glm::vec3(x - 500 + 1, noiseMap[y + 1][x + 1] * 45, y - 500 + 1);
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
            normal = calculateSurfaceNormal(bottomLeft, topRight, topLeft);
            currentIndex = setVertex(currentIndex, bottomLeft[0], bottomLeft[1], bottomLeft[2], r, g, b, normal[0], normal[1], normal[2], 0.0f, 0.0f);
            currentIndex = setVertex(currentIndex, topRight[0], topRight[1], topRight[2], r, g, b, normal[0], normal[1], normal[2], 1.0f, 1.0f);
            currentIndex = setVertex(currentIndex, topLeft[0], topLeft[1], topLeft[2], r, g, b, normal[0], normal[1], normal[2], 0.0f, 1.0f);
            normal = calculateSurfaceNormal(bottomLeft, bottomRight, topRight);
            currentIndex = setVertex(currentIndex, bottomLeft[0], bottomLeft[1], bottomLeft[2], r, g, b, normal[0], normal[1], normal[2], 0.0f, 0.0f);
            currentIndex = setVertex(currentIndex, topRight[0], topRight[1], topRight[2], r, g, b, normal[0], normal[1], normal[2], 1.0f, 1.0f);
            currentIndex = setVertex(currentIndex, bottomRight[0], bottomRight[1], bottomRight[2], r, g, b, normal[0], normal[1], normal[2], 1.0f, 0.0f);
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
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

    unsigned int diffuseMap = loadTexture(FileSystem::getPath("images/white.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("images/white.png").c_str());
    srand(time(0));
    Light light;
    for (int i = 0; i < 100; i++) {
        float r = sin(rand()) * 0.5 + 0.5;
        float b = sin(rand()) * 0.5 + 0.5;
        float g = sin(rand()) * 0.5 + 0.5;
        light = Light(2.0, glm::vec3(sin(rand()) * 200, 30, sin(rand()) * 200), glm::vec3(r / 20, g / 20, b / 20), glm::vec3(r / 1.25, g / 1.25, b / 1.25), glm::vec3(r, g, b), 50, lightingShader);
        light.init(lightingShader);
        lights[light.ID] = light;
    }

    while(!glfwWindowShouldClose(window))   {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        // cout << "\n" << deltaTime;
        const char* text = to_string(1000 / deltaTime / 1000).c_str();
        glfwSetWindowTitle(window, text);
        cameraSpeed = 500.5f * deltaTime;
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
        float timeValue = glfwGetTime();

        lightingShader.use();
        lightingShader.setVec3("viewPos", cameraPos[0], cameraPos[1], cameraPos[2]); 
        lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lightingShader.setFloat("material.shininess", 32.0f);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        lightingShader.setVec3("dirLight.direction", -0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 10000.0f);

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
        glDrawArrays(GL_TRIANGLES, 0, 66000000 / 6); 

        for (int i = 0; i < pointLightCount; i++) {
            lights[i].setPosition(glm::vec3(lights[i].position.x + sin(timeValue), lights[i].position.y, lights[i].position.z + cos(timeValue)), lightingShader);
        }

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