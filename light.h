#ifndef LIGHT_H
#define LIGHT_H
#include <cstddef>
#include <string>
#include "shader.h"
#include "stdint.h"
#include "inttypes.h"
#include "stdint.h"
#include "glad.h" 
#include "khrplatform.h"
#include <GLFW/glfw3.h>
#include "inttypes.h"
#include <fstream>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "helpers.h"


using namespace std;

class Light {
public:
    int ID;
    glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
    
    float constant = 1.0;
    float linear;
    float quadratic;  
    float brightness;
    float scale;

    glm::vec3 ambientColor = glm::vec3(0.05, 0.05, 0.05);
    glm::vec3 diffuseColor = glm::vec3(0.8, 0.8, 0.8);
    glm::vec3 specularColor = glm::vec3(1.0, 1.0, 1.0);
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Light(float scaleArg = 1.0, glm::vec3 pos = glm::vec3(1.0, 1.0, 1.0), glm::vec3 ambientArg = glm::vec3(1.0, 1.0, 1.0), glm::vec3 diffuseArg = glm::vec3(1.0, 1.0, 1.0), glm::vec3 specularArg = glm::vec3(1.0, 1.0, 1.0), float brightnessArg = 1.0, Shader shaderArg = Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl"), float constantArg = 1.0, float linearArg = 0.09, float quadraticArg = 0.032) {
        setScale(scaleArg);
        setPosition(pos, shaderArg);
        setBrightness(brightnessArg);
        setAmbientColor(ambientArg, shaderArg);
        setDiffuseColor(diffuseArg, shaderArg);
        setSpecularColor(specularArg, shaderArg);
        setConstant(constantArg, shaderArg);
        setLinear(linearArg, shaderArg);
        setQuadratic(quadraticArg, shaderArg);
    }
    // ------------------------------------------------------------------------
    void init(Shader shaderArg) { 
        shaderArg.use();
        ID = pointLightCount;
        pointLightCount += 1;
        shaderArg.setInt("pointLightCount", pointLightCount);
        setScale(scale);
        setPosition(position, shaderArg);
        setBrightness(brightness);
        setAmbientColor(ambientColor, shaderArg);
        setDiffuseColor(diffuseColor, shaderArg);
        setSpecularColor(specularColor, shaderArg);
        setConstant(constant, shaderArg);
        setLinear(linear, shaderArg);
        setQuadratic(quadratic, shaderArg);
    }
    // ------------------------------------------------------------------------
    void setPosition(glm::vec3 positionArg, Shader shaderArg) { 
        position = positionArg;
        shaderArg.setVec3("pointLights[" + to_string(ID) + "].position", position.x, position.y, position.z);
    }
    // ------------------------------------------------------------------------
    void setAmbientColor(glm::vec3 ambientArg, Shader shaderArg) { 
        ambientColor = ambientArg;
        shaderArg.setVec3("pointLights[" + to_string(ID) + "].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
    }
    // ------------------------------------------------------------------------
    void setDiffuseColor(glm::vec3 diffuseArg, Shader shaderArg) { 
        diffuseColor = diffuseArg;
        shaderArg.setVec3("pointLights[" + to_string(ID) + "].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
    }
    // ------------------------------------------------------------------------
    void setSpecularColor(glm::vec3 specularArg, Shader shaderArg) { 
        specularColor = specularArg;
        shaderArg.setVec3("pointLights[" + to_string(ID) + "].specular", specularColor.x, specularColor.y, specularColor.z);
    }
    // ------------------------------------------------------------------------
    void setConstant(float constantArg, Shader shaderArg) { 
        constant = constantArg;
        shaderArg.setFloat("pointLights[" + to_string(ID) + "].constant", constant / brightness);
    }
    // ------------------------------------------------------------------------
    void setLinear(float linearArg, Shader shaderArg) { 
        linear = linearArg;
        shaderArg.setFloat("pointLights[" + to_string(ID) + "].linear", linear / brightness);
    }
    // ------------------------------------------------------------------------
    void setQuadratic(float quadraticArg, Shader shaderArg) { 
        quadratic = quadraticArg;
        shaderArg.setFloat("pointLights[" + to_string(ID) + "].quadratic", quadratic / brightness);
    }
    // ------------------------------------------------------------------------
    void setBrightness(float brightnessArg) { 
        brightness = brightnessArg;
    }
    // ------------------------------------------------------------------------
    void setScale(float scaleArg) { 
        scale = scaleArg;
    }
    // ------------------------------------------------------------------------
    void render(Shader shaderArg) { 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(scale)); // Make it a smaller cube
        shaderArg.setMat4("model", model);
        shaderArg.setVec3("color", specularColor.x, specularColor.y, specularColor.z);
        glDrawArrays(GL_TRIANGLES, 0, 72);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    
};
#endif
