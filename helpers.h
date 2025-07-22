#include <cstddef>

#include <string>
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
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION

#ifndef HELPERS
#define HELPERS

using namespace std;
// struct rectWTex {
//     SDL_Rect SDLRect;
//     SDL_Texture* texture;
// };
extern int scroll;
extern int pointLightCount;
// extern bool running;
// extern const uint8_t* keys;
// extern SDL_Window* window;
// extern SDL_Renderer *renderer;
// void handleControls();
// rectWTex getRectFromPath(char *path, int x, int y, int w, int h);
extern void processInput(GLFWwindow *window);
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
extern glm::vec3 calculateSurfaceNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

extern unsigned int loadTexture(char const * path);
extern int glfwInits();
extern int width;
extern int height;
extern int windowWidth;
extern int windowHeight;
extern const char* readFile(string path);
extern glm::vec3 cameraPos;
extern glm::vec3 cameraTarget;
extern glm::vec3 cameraDirection;
extern glm::vec3 up; 
extern glm::vec3 cameraRight;
extern glm::vec3 cameraUp;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern float fov;
extern int chunkSize;
extern unsigned int ChunkVBO, ChunkVAO;
extern float cameraSpeed;
extern float useTime;
extern float setUniformTime;
extern float dispatchTime;
extern float transferTime;
extern float verticesTime;
extern float indicesTime;
extern float deltaTime;	// Time between current frame and last frame
extern float lastFrame; // Time of last frame
extern GLFWwindow* window;
#endif