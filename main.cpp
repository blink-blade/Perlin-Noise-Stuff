#include "chunk.h"
#include "glm/detail/type_vec.hpp"
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
// vector<vector<Chunk>> chunks(100, vector<Chunk>());
vector<vector<Chunk>> chunks;
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
std::vector<glm::vec3> normals(width * height);
// float vertices[] = {
//     -0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
//     0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f
// };
int main()
{
    glfwInits();
    Shader lightingShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Shader lightCubeShader("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");
    lightingShader.use(); // don't forget to activate/use the shader before setting uniforms!

    // first, configure the cube's VAO (and VBO)
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); 

    int nrAttributes, max;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;
    cout << "Maximum texture size supported: " << max  << endl;
    Light lights[1000];
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("width", width);
    lightingShader.setInt("height", height);
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("images/spritesheet.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("images/spritesheetspecularmap.png").c_str());
    srand(time(0));
    Light light;
    // for (int i = 0; i < 1; i++) {
    //     // float r = round(sin(rand()) * 0.5 + 0.5);
    //     // float g = round(sin(rand()) * 0.5 + 0.5);
    //     // float b = round(sin(rand()) * 0.5 + 0.5);
    //     float r = 255 / 255;
    //     float g = 204 / 255;
    //     float b = 51 / 255;
    //     light = Light(2.0, glm::vec3(500, 30, 500), glm::vec3(r / 20, g / 20, b / 20), glm::vec3(r / 1.25, g / 1.25, b / 1.25), glm::vec3(r, g, b), 1000, lightingShader);
    //     light.init(lightingShader);
    //     lights[light.ID] = light;
    // }
    const char* text = to_string(1000 / deltaTime / 1000).c_str();
    int jeffy = 0;
    float deltaTimeList[50];

    lightingShader.use(); // don't forget to activate/use the shader before setting uniforms!

    float timeTakenToStart = glfwGetTime();
    for (int y = 0; y < height / chunkSize; y++) {
        chunks.push_back(vector<Chunk>());
        for (int x = 0; x < width / chunkSize; x++) {
            chunks[y].push_back(Chunk(glm::vec2(x, y)));
            // glBufferData(GL_ARRAY_BUFFER, chunks[y][x].vertices.size() * sizeof(float), chunks[y][x].vertices.data(), GL_STATIC_DRAW);
        }   
    }
    cout << "Time was: " << glfwGetTime() - timeTakenToStart << "\n";
    

    // glUniform1i(glGetUniformLocation(lightingShader.ID, "texture1"), 0);
    // unsigned int textureID;
    // glGenTextures(1, &textureID);
    // glBindTexture(GL_TEXTURE_2D, textureID);

    // // Upload the float data
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, normals.data());
    // GLint max_texture_size;
    // // Set texture filtering and wrapping
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // or GL_LINEAR
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, textureID);


        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 10000.0f);


        lightingShader.use();
        lightingShader.setVec3("viewPos", cameraPos[0], cameraPos[1], cameraPos[2]); 
        lightingShader.setFloat("material.shininess", 1.0f);
        lightingShader.setVec3("dirLight.direction", -0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.05f * 1, 0.05f * 1, 0.05f * 1);
        lightingShader.setVec3("dirLight.diffuse", 0.4f * 1, 0.4f * 1, 0.4f * 1);
        lightingShader.setVec3("dirLight.specular", 0.5f * 1, 0.5f * 1, 0.5f * 1);
        // for (int i = 0; i < pointLightCount; i++) {
        //     lights[i].setPosition(glm::vec3(lights[i].position.x + sin(timeValue), lights[i].position.y, lights[i].position.z + cos(timeValue)), lightingShader);
        // }
        float greenValue = sin(timeValue);
        lightingShader.setFloat("timeOffsetColor", sin(timeValue));
        int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");
        glUniformMatrix4fv( projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        int timeLoc = glGetUniformLocation(lightingShader.ID, "time");
        glUniform1f(timeLoc, (float)glfwGetTime());


        // draw the object
        
        // cout << "hi";
        for (int y = 0; y < height / chunkSize; y++) {
            for (int x = 0; x < width / chunkSize; x++) {
                glBindVertexArray(chunks[y][x].VAO);
                glDrawArrays(GL_TRIANGLES, 0, chunks[y][x].vertices.size() / 9);
            }   
        }
        
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 54000000 / 9, 1); 

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(cubeVAO);
        // for (int i = 0; i < pointLightCount; i++) {
        //     lights[i].setPosition(glm::vec3(lights[i].position.x + sin(timeValue) * 5, lights[i].position.y + sin(timeValue) * 5, lights[i].position.z + sin(timeValue) * 5), lightingShader);
        //     lights[i].render(lightCubeShader);
        // }


        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &ChunkVAO);
    glDeleteBuffers(1, &ChunkVBO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);     
    glfwTerminate();

    return 0;
}