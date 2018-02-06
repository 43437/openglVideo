#include "WindowView.hpp"

int WindowView::createWindow()
{
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  
//   glViewport(0.0, 0.0, 1024, 768);
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  
//   std::thread t(&WindowView::refresh, this);
//   t.detach();
//   
  return 0;
/*
  static const GLfloat g_vertex_buffer_data[] = { 
	  -1.0f, -1.0f, 0.0f,
	    1.0f, -1.0f, 0.0f,
	    1.0f,  1.0f, 0.0f,
	    -1.0f, 1.0f, 0.0f
  };

  static const GLfloat uvs[] = { 
	    0.0f, 1.0f,
	    1.0f, 1.0f,
	    1.0f,  0.0f,
	    0.0f, 0.0f
  };

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(glm::vec3), g_vertex_buffer_data, GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
  TextureID = glGetUniformLocation(programID, "myTextureSampler");*/
}

void WindowView::refresh()
{
  do
  {
//     draw();
    glfwPollEvents();
  }while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0 );
}

WindowView::WindowView(uint64_t pixel_w, uint64_t pixel_h):pixel_w(pixel_w), pixel_h(pixel_h)
{
  createWindow();
  InitShaders();
}

int WindowView::windowHandle(uint8_t *buffer)
{
  this->buf = buffer;
  
  plane[0] = buf;  
  plane[1] = plane[0] + pixel_w*pixel_h;  
  plane[2] = plane[1] + pixel_w*pixel_h/4;
  
  /*
  std::thread t(&WindowView::refresh, this);
  t.detach();*/
}

void WindowView::draw(int i)
{
  //Clear  
  i/=10;
  i%=256;
  glClearColor(0,0,i,0.0);  
  glClear(GL_COLOR_BUFFER_BIT);  
 
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
	  0,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	  3,                                // size : U+V => 2
	  GL_FLOAT,                         // type
	  GL_FALSE,                         // normalized?
	  0,                                // stride
	  (void*)0                          // array buffer offset
  );
  
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glVertexAttribPointer(
	  1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	  2,                                // size : U+V => 2
	  GL_FLOAT,                         // type
	  GL_FALSE,                         // normalized?
	  0,                                // stride
	  (void*)0                          // array buffer offset
  );
		
  //Y  
  glActiveTexture(GL_TEXTURE0);  
  glBindTexture(GL_TEXTURE_2D, id_y);  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w, pixel_h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[0]);   
//   glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  glUniform1i(textureUniformY, 0);      

  //U  
  glActiveTexture(GL_TEXTURE1);  
  glBindTexture(GL_TEXTURE_2D, id_u);  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w/2, pixel_h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[1]);         
  glUniform1i(textureUniformU, 1);  

  //V  
  glActiveTexture(GL_TEXTURE2);  
  glBindTexture(GL_TEXTURE_2D, id_v);  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w/2, pixel_h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[2]);      
  glUniform1i(textureUniformV, 2);     

  // Draw  
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  
  // Show  
  //Double  
  glfwSwapBuffers(window);
}

char* WindowView::textFileRead(char * filename)  
{  
    char *s = (char *)malloc(8000);  
    memset(s, 0, 8000);  
    FILE *infile = std::fopen(filename, "rb");  
    int len = std::fread(s, 1, 8000, infile);  
    std::fclose(infile);  
    s[len] = 0;  
    return s;  
}  

//Init Shader  
void WindowView::InitShaders()  
{  
    GLint vertCompiled, fragCompiled, linked;  
      
    GLint v, f;  
    const char *vs,*fs;  
    //Shader: step1  
    v = glCreateShader(GL_VERTEX_SHADER);  
    f = glCreateShader(GL_FRAGMENT_SHADER);  
    //Get source code  
    vs = textFileRead("Shader.vsh");  
    fs = textFileRead("Shader.fsh");  
    //Shader: step2  
    glShaderSource(v, 1, &vs,NULL);  
    glShaderSource(f, 1, &fs,NULL);  
    //Shader: step3  
    glCompileShader(v);  
    //Debug  
    glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);  
    glCompileShader(f);  
    glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);  
  
    //Program: Step1  
    GLuint p = glCreateProgram();   
    //Program: Step2  
    glAttachShader(p,v);  
    glAttachShader(p,f);   
  
   /* glBindAttribLocation(p, ATTRIB_VERTEX, "vertexIn");  
    glBindAttribLocation(p, ATTRIB_TEXTURE, "textureIn"); */ 
    //Program: Step3  
    glLinkProgram(p);  
    //Debug  
    glGetProgramiv(p, GL_LINK_STATUS, &linked);    
    //Program: Step4  
    glUseProgram(p);  
  
  
    //Get Uniform Variables Location  
    textureUniformY = glGetUniformLocation(p, "tex_y");  
    textureUniformU = glGetUniformLocation(p, "tex_u");  
    textureUniformV = glGetUniformLocation(p, "tex_v");   
  
#if TEXTURE_ROTATE  
    static const GLfloat vertexVertices[] = {  
        -1.0f, -0.5f,  
         0.5f, -1.0f,  
        -0.5f,  1.0f,  
         1.0f,  0.5f,  
    };      
#else  
    static const GLfloat vertexVertices[] = {  
        -1.0f, -1.0f, 0.0f, 
        1.0f, -1.0f,  0.0f, 
        -1.0f,  1.0f,  0.0f, 
        1.0f,  1.0f,  0.0f
    };      
#endif  
  
#if TEXTURE_HALF  
    static const GLfloat textureVertices[] = {  
        0.0f,  1.0f,  
        0.5f,  1.0f,  
        0.0f,  0.0f,  
        0.5f,  0.0f,  
    };   
#else  
    static const GLfloat textureVertices[] = {  
        0.0f,  1.0f,  
        1.0f,  1.0f,  
        0.0f,  0.0f,  
        1.0f,  0.0f,  
    };   
#endif  
//     //Set Arrays  
//     glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, vertexVertices);  
//     //Enable it  
//     glEnableVertexAttribArray(ATTRIB_VERTEX);      
//     
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVertices), vertexVertices, GL_STATIC_DRAW);
	
    /*glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);  
    glEnableVertexAttribArray(ATTRIB_TEXTURE); */ 
  
  
    //Init Texture  
    glGenTextures(1, &id_y);   
    glBindTexture(GL_TEXTURE_2D, id_y);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
      
    glGenTextures(1, &id_u);  
    glBindTexture(GL_TEXTURE_2D, id_u);     
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
      
    glGenTextures(1, &id_v);   
    glBindTexture(GL_TEXTURE_2D, id_v);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  

}





