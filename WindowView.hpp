#ifndef __WINDOW_VIEW__
#define __WINDOW_VIEW__

#include <cstdio>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdio>
#include <iostream>

extern "C"{
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>  
}

// Include GLM
#include <glm/glm.hpp>  


using namespace glm;

#define ATTRIB_VERTEX 3  
#define ATTRIB_TEXTURE 4 

class WindowView{
  
  GLFWwindow* window;
  
  GLuint id_y;
  GLuint id_u;
  GLuint id_v; // Texture id  
  GLuint textureUniformY, textureUniformU,textureUniformV; 
  GLuint vertexbuffer, uvbuffer;
  
  uint8_t* buf;
  uint8_t *plane[3];  
  
  uint64_t pixel_w, pixel_h;

  int createWindow();
  void InitShaders();
  char *textFileRead(char * filename);
  void refresh();
public:
  WindowView(uint64_t pixel_w, uint64_t pixel_h);
  int windowHandle(uint8_t *buffer);
  void draw(int i);
};

#endif