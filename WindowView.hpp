#ifndef __WINDOW_VIEW__
#define __WINDOW_VIEW__

#include <stdio.h>
#include <stdlib.h>

extern "C"{
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>  
}

// Include GLM
#include <glm/glm.hpp>  


using namespace glm;

class WindowView{
  
  GLFWwindow* window;
  GLuint vertexbuffer;
  int createWindow();
  int windowHandle();
public:
  WindowView();
};
#endif