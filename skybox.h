#ifndef SKYBOX_H
#define SKYBOX_H

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
#include "shader.h"
#include "filesystem.h"

using namespace std;

class SkyBox {
public:
    // x, y, z, tx, ty
    vector<float> vertices;
    unsigned int textureID;
    unsigned int VAO, VBO;
    int scale = 10000;

    // ------------------------------------------------------------------------
    SkyBox(Shader shaderArg) {
        setVertices();
        // Rendering stuff.
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        shaderArg.use();
        shaderArg.setInt("skyboxTexture", 2);
        textureID = loadTexture(FileSystem::getPath("images/skybox.png").c_str());

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    }
    void setVertices() {
        int currentIndex = 0;
        float cubeVertices[72]= {
                // Back side
                -0.5f, -0.5f, -0.5f, 0.0, 0.3333f,
                0.5f, -0.5f, -0.5f, 0.25f, 0.3333f,
                0.5f,  0.5f, -0.5f, 0.25f, 0.6666f,
                0.5f,  0.5f, -0.5f, 0.25f, 0.6666f,
                -0.5f,  0.5f, -0.5f, 0.0, 0.6666f,
                -0.5f, -0.5f, -0.5f, 0.0, 0.3333f,

                // Front side
                -0.5f, -0.5f,  0.5f, 0.5f, 0.3333f,
                0.5f, -0.5f,  0.5f, 0.75f, 0.3333f,
                0.5f,  0.5f,  0.5f, 0.75f, 0.6666f,
                0.5f,  0.5f,  0.5f, 0.75f, 0.6666f,
                -0.5f,  0.5f,  0.5f, 0.5f, 0.6666f,
                -0.5f, -0.5f,  0.5f, 0.5f, 0.3333f

                // // Left side
                // -0.5f,  0.5f,  0.5f, 
                // -0.5f,  0.5f, -0.5f, 
                // -0.5f, -0.5f, -0.5f, 
                // -0.5f, -0.5f, -0.5f, 
                // -0.5f, -0.5f,  0.5f, 
                // -0.5f,  0.5f,  0.5f, 

                // // Right side
                // 0.5f,  0.5f,  0.5f,  
                // 0.5f,  0.5f, -0.5f,  
                // 0.5f, -0.5f, -0.5f,  
                // 0.5f, -0.5f, -0.5f,  
                // 0.5f, -0.5f,  0.5f,  
                // 0.5f,  0.5f,  0.5f,  

                // // Bottom side
                // -0.5f, -0.5f, -0.5f, 
                // 0.5f, -0.5f, -0.5f,  
                // 0.5f, -0.5f,  0.5f,  
                // 0.5f, -0.5f,  0.5f,  
                // -0.5f, -0.5f,  0.5f, 
                // -0.5f, -0.5f, -0.5f, 
                
                // // Top side
                // -0.5f,  0.5f, -0.5f, 
                // 0.5f,  0.5f, -0.5f,  
                // 0.5f,  0.5f,  0.5f,  
                // 0.5f,  0.5f,  0.5f,  
                // -0.5f,  0.5f,  0.5f, 
                // -0.5f,  0.5f, -0.5f
        };
        for (int i = 0; i < 72; i++) {
            vertices.push_back(cubeVertices[i]);
        }
    };

    void render(Shader shaderArg) { 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(scale)); // Make it a smaller cube
        shaderArg.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 72);
    }
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    
};

#endif