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

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
float vertices[54000000]; //width * height * 6 * 6.       * 6 for (x, y, z, r, g, b) and * 6 for six vertices on each face.
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

int setVertex(int index, float x, float y, float z, float r, float b, float g, float nVX, float nVY, float nVZ) {
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
    noiseMap = generateNoiseMap(width + 1, width + 1, 5, 2, 6, 1, 0, 0, 0, 0, 0, 123);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // I believe we need to go in a counter-clockwise order of vertices.
            bottomLeft = glm::vec3(x, noiseMap[y][x] * 45, y);
            bottomRight = glm::vec3(x + 1, noiseMap[y][x + 1] * 45, y);
            topLeft = glm::vec3(x, noiseMap[y + 1][x] * 45, y + 1);
            topRight = glm::vec3(x + 1, noiseMap[y + 1][x + 1] * 45, y + 1);
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
            currentIndex = setVertex(currentIndex, bottomLeft[0], bottomLeft[1], bottomLeft[2], r, g, b, normal[0], normal[1], normal[2]);
            currentIndex = setVertex(currentIndex, topRight[0], topRight[1], topRight[2], r, g, b, normal[0], normal[1], normal[2]);
            currentIndex = setVertex(currentIndex, topLeft[0], topLeft[1], topLeft[2], r, g, b, normal[0], normal[1], normal[2]);
            currentIndex = setVertex(currentIndex, bottomLeft[0], bottomLeft[1], bottomLeft[2], r, g, b, normal[0], normal[1], normal[2]);
            currentIndex = setVertex(currentIndex, topRight[0], topRight[1], topRight[2], r, g, b, normal[0], normal[1], normal[2]);
            currentIndex = setVertex(currentIndex, bottomRight[0], bottomRight[1], bottomRight[2], r, g, b, normal[0], normal[1], normal[2]);
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


    glUniform1i(glGetUniformLocation(lightingShader.ID, "texture1"), 0);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


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

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos[0], lightPos[1], lightPos[2]); 

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 10000.0f);

        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue);
        lightingShader.setFloat("timeOffsetColor", sin(timeValue));
        int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        int timeLoc = glGetUniformLocation(lightingShader.ID, "time");
        glUniform1f(timeLoc, (float)glfwGetTime());

        // draw the object
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 54000000); 

        // world transformation
        
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 216);
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