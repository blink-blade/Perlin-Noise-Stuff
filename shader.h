

#ifndef SHADER_H
#define SHADER_H

#include "glad.h" 
#include "glm/detail/type_mat.hpp"
#include "glm/glm.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

using namespace std;

class Shader {
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string tControlCode;
        std::string tEvalCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile; 
        std::ifstream tCShaderFile;
        std::ifstream tEShaderFile; 
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        if(tessControlPath != nullptr) {
            tCShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            tEShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        }
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();			
            // if geometry shader path is present, also load a geometry shader
            if(tessControlPath != nullptr) {
                tCShaderFile.open(tessControlPath);
                tEShaderFile.open(tessEvalPath);
                std::stringstream tCShaderStream, tEShaderStream;
                // read file's buffer contents into streams
                tCShaderStream << tCShaderFile.rdbuf();
                tEShaderStream << tEShaderFile.rdbuf();		
                // close file handlers
                tCShaderFile.close();
                tEShaderFile.close();
                // convert stream into string
                tControlCode = tCShaderStream.str();
                tEvalCode = tEShaderStream.str();		
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        const char* tCShaderCode, *tEShaderCode;
        if(tessControlPath != nullptr) {
            tCShaderCode = tControlCode.c_str();
            tEShaderCode = tEvalCode.c_str();
        }
        
        // 2. compile shaders
        unsigned int vertex, fragment, tessControl, tessEval;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        if(tessControlPath != nullptr)
        {
            // vertex shader
            tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tessControl, 1, &tCShaderCode, NULL);
            glCompileShader(tessControl);
            checkCompileErrors(tessControl, "TESSELATION CONTROL");
            // fragment Shader
            tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tessEval, 1, &tEShaderCode, NULL);
            glCompileShader(tessEval);
            checkCompileErrors(tessEval, "TESSELATION EVALUTATION");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (tessControlPath != nullptr) {
            glAttachShader(ID, tessControl);
            glAttachShader(ID, tessEval);
        }
            
        glLinkProgram(ID);
        // checkCompileErrors(ID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (tessControlPath != nullptr) {
            glDeleteShader(tessControl);
            glDeleteShader(tessEval);
        }
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const string &name, float x, float y) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(glm::vec2(x, y))); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const string &name, float x, float y, float z) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(glm::vec3(x, y, z))); 
    }
    // ------------------------------------------------------------------------
    void setMat4(const string &name, glm::mat4 matrix) const
    { 
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix)); 
    }
    // ------------------------------------------------------------------------

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];

        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        
    }
};
#endif