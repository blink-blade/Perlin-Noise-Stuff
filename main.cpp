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
int width = 500;
int height = 500;
float vertices[9000000]; //width * height * 6 * 6.       * 6 for (x, y, z, r, g, b) and * 6 for six vertices on each face.
// float vertices[] = {
//     -0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f
// };

void makeFaces() {
    int faceIndex, currentIndex, firstTriangleIndex, secondTriangleIndex;
    int bottomLeft[2], bottomRight[2], topLeft[2], topRight[2];
    currentIndex = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // There will be 6 * 6 times the amount of items in the vertices list than the amount of faces, because -
            // each face is 6 vertices, each vertex has six attributes (x, y, z, r, g, b).
            faceIndex = x + width*y;
            // I believe we need to go in a counter-clockwise order of vertices.
            bottomLeft[0] = x; bottomLeft[1] = y;
            bottomRight[0] = x + 1; bottomRight[1] = y;
            topLeft[0] = x; topLeft[1] = y + 1;
            topRight[0] = x + 1; topRight[1] = y + 1;
            
            // XYZ of each vertex.
            vertices[currentIndex] = bottomLeft[0];
            currentIndex += 1;
            vertices[currentIndex] = 0;
            currentIndex += 1;
            vertices[currentIndex] = bottomLeft[1];
            currentIndex += 1;
            vertices[currentIndex] = (float)x / width;
            currentIndex += 1;
            vertices[currentIndex] = (float)y / width;
            currentIndex += 1;
            vertices[currentIndex] = 1;
            currentIndex += 1;

            vertices[currentIndex] = topRight[0];
            currentIndex += 1;
            vertices[currentIndex] = 0;
            currentIndex += 1;
            vertices[currentIndex] = topRight[1];
            currentIndex += 1;
            vertices[currentIndex] = (float)x / width;
            currentIndex += 1;
            vertices[currentIndex] = (float)y / width;
            currentIndex += 1;
            vertices[currentIndex] = 1;
            currentIndex += 1;

            vertices[currentIndex] = topLeft[0];
            currentIndex += 1;
            vertices[currentIndex] = 0;
            currentIndex += 1;
            vertices[currentIndex] = topLeft[1];
            currentIndex += 1;
            vertices[currentIndex] = (float)x / width;
            currentIndex += 1;
            vertices[currentIndex] = (float)y / width;
            currentIndex += 1;
            vertices[currentIndex] = 1;
            currentIndex += 1;



            vertices[currentIndex] = bottomLeft[0];
            currentIndex += 1;
            vertices[currentIndex] = 0;
            currentIndex += 1;
            vertices[currentIndex] = bottomLeft[1];
            currentIndex += 1;
            vertices[currentIndex] = (float)x / width;
            currentIndex += 1;
            vertices[currentIndex] = (float)y / width;
            currentIndex += 1;
            vertices[currentIndex] = 1;
            currentIndex += 1;

            vertices[currentIndex] = bottomRight[0];
            currentIndex += 1;
            vertices[currentIndex] = 0;
            currentIndex += 1;
            vertices[currentIndex] = bottomRight[1];
            currentIndex += 1;
            vertices[currentIndex] = (float)x / width;
            currentIndex += 1;
            vertices[currentIndex] = (float)y / width;
            currentIndex += 1;
            vertices[currentIndex] = 1;
            currentIndex += 1;

            vertices[currentIndex] = topRight[0];
            currentIndex += 1;
            vertices[currentIndex] = 0;
            currentIndex += 1;
            vertices[currentIndex] = topRight[1];
            currentIndex += 1;
            vertices[currentIndex] = (float)x / width;
            currentIndex += 1;
            vertices[currentIndex] = (float)y / width;
            currentIndex += 1;
            vertices[currentIndex] = 1;
            currentIndex += 1;
        }   
    }
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

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;




    while(!glfwWindowShouldClose(window))   {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        cameraSpeed = 200.5f * deltaTime;
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
        glDrawArrays(GL_TRIANGLES, 0, 9000000); 
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