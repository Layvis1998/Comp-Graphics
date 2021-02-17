#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <chrono> 

using namespace std;
using namespace std::chrono ;


//starting window size values
#define SCR_WIDTH 470
#define SCR_HEIGHT 470
#define HALF_SCR_WIDTH 235
#define HALF_SCR_HEIGHT 235

//Variables for the fragment shader
float eye[3] = {0.0, 0.0, 0.0};
float cp_start[3] = {0.0, 0.0, 1.5};     //starting central point
float lbp_start[3] = {-1.0, -1.0, 1.5};  //starting left bottom point     
float ltp_start[3] = {-1.0, 1.0, 1.5};   //starting left top point
float rbp_start[3] = {1.0, -1.0, 1.5};   //starting right bottom point
float rtp_start[3] = {1.0, 1.0, 1.5};    //starting right top point
float lbp[3] = {-1.0, -1.0, 1.5};  //left bottom point     
float ltp[3] = {-1.0, 1.0, 1.5};   //left top point
float rbp[3] = {1.0, -1.0, 1.5};   //right bottom point
float rtp[3] = {1.0, 1.0, 1.5};    //right top point 
float vert_normale_start[3];
float hor_normale_start[3];
float vert_normale[3];
float hor_normale[3];

float alpha = 0;
float beta = 0;
float rot_matrix[9];
time_point<high_resolution_clock> start, stop;
const int fps_window_size = 10;
int fps_counter = 0;
double fps_total_time = 0;
uint64_t total_measures = 0;
double total_time = 0;


void StartTimer()
{
  start = high_resolution_clock::now();
}

void StopTimer(const char* msg)
{
  stop = high_resolution_clock::now();
  std::chrono::duration<double> diff = stop - start;
  cout << msg << "\n";
  cout << diff.count() << "\n";
}


void StopTimerFPS()
{
  stop = high_resolution_clock::now();
  std::chrono::duration<double> diff = stop - start;
  double past = diff.count();
  if (fps_counter == fps_window_size - 1)
  {
    double fps = fps_window_size / fps_total_time;
    cout << "\r" << fps;
    cout << flush;
    total_time += fps_total_time;
    total_measures += fps_window_size;
    fps_total_time = 0;
  }  
  fps_counter = (fps_counter + 1) % fps_window_size;
  fps_total_time += past;
}


void Create_rot_matrix()
{
  float hor_rot_matrix[9] =
  {
    cos(alpha), 0, -sin(alpha), 0, 1, 0, sin(alpha), 0, cos(alpha)
  };
  
  float vert_rot_matrix[9] =
  {
    1, 0, 0, 0, cos(beta), -sin(beta), 0, sin(beta), cos(beta)
  };
   
  for (uint8_t i = 0; i < 3; i++)
  {
    for (uint8_t j = 0; j < 3; j++)
    {
	  uint32_t height = i * 3;
      rot_matrix[height + j] = 0; 
      for (uint8_t k = 0; k < 3; k++)
      {
        rot_matrix[height + j] += hor_rot_matrix[height + k]
         * vert_rot_matrix[k * 3 + j];	
      }
    }
  }
}

void cursorPositionCallback(GLFWwindow *window, double pos_x, double pos_y)
{
  float speed = 0.00018;
  alpha -= speed * (int(pos_x) - HALF_SCR_WIDTH);
  beta += speed * (int(pos_y) - HALF_SCR_HEIGHT);
  if (beta >= M_PI_2)
    beta = M_PI_2;
  if (beta <= - M_PI_2)
    beta = (- M_PI_2);
}

void Mv_mult_3(float * matrix, float* arr)
{
  float aux[3];
  for (uint32_t i = 0; i < 3; i++)
    aux[i] = arr[i];
    
  for (uint32_t i = 0; i < 3; i++)
  {
    arr[i] =  matrix[i * 3] * aux[0] + matrix[i * 3 + 1] * aux[1] 
      + matrix[i * 3 + 2] * aux[2];    
  }
}

void update_coordinates()
{
  for (uint8_t i = 0; i < 3; i++)
  {
    lbp[i] = lbp_start[i];
    ltp[i] = ltp_start[i];
    rtp[i] = rtp_start[i];
    rbp[i] = rbp_start[i];
    hor_normale[i] = hor_normale_start[i];
    vert_normale[i] = vert_normale_start[i];
  }
  Create_rot_matrix();
  Mv_mult_3(rot_matrix, hor_normale);
  Mv_mult_3(rot_matrix, vert_normale);
  Mv_mult_3(rot_matrix, lbp);
  Mv_mult_3(rot_matrix, ltp);
  Mv_mult_3(rot_matrix, rtp);
  Mv_mult_3(rot_matrix, rbp);
}

void processInput(GLFWwindow *window)
{
	
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
   
  float speed = 0.13;  
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    eye[0] += vert_normale[0] * speed;
    eye[1] += vert_normale[1] * speed;
    eye[2] += vert_normale[2] * speed;
  }
  
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    eye[0] -= vert_normale[0] * speed;
    eye[1] -= vert_normale[1] * speed;
    eye[2] -= vert_normale[2] * speed;  
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    eye[0] += hor_normale[0] * speed;
    eye[1] += hor_normale[1] * speed;
    eye[2] += hor_normale[2] * speed; 
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    eye[0] -= hor_normale[0] * speed;
    eye[1] -= hor_normale[1] * speed;
    eye[2] -= hor_normale[2] * speed;   
  }  
}

void Check_Confines()
{
  //Check scene confines
  if (eye[0] >= 38)
    eye[0] = 38;
  if (eye[1] >= 38)
    eye[1] = 38;  
  if (eye[2] >= 38)
    eye[2] = 38;
  if (eye[0] <= -80)
    eye[0] = -80;
  if (eye[1] <= -38)
    eye[1] = -38;  
  if (eye[2] <= -38)
    eye[2] = -38;
 /* //Check portal confines  
  if
  ( ((eye[0] >= 19.9) && (eye[0] <= 20.1)) &&
    ((eye[1] >= -1) && (eye[1] <= 5)) &&  	
    ((eye[2] >= 3.5) && (eye[2] <= 6.5)) )
  {
    eye[0] = 0;
    eye[1] = 0;
    eye[2] = -37;
  }
  */
}

//each time the window size is changed, this function is being called
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

int main()
{
  vert_normale_start[0] = cp_start[0] - eye[0];
  vert_normale_start[1] = cp_start[1] - eye[1];
  vert_normale_start[2] = cp_start[2] - eye[2];
  hor_normale_start[0] = 1;
  hor_normale_start[1] = 0;
  hor_normale_start[2] = 0;

  StartTimer();
  
  //initializing glfw
  glfwInit();                                      
  //choosing glfw version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);    
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  
  //choosing OpenGL mod
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfw: creating window
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
    "Ilya Karandeev's computer graphics", NULL, NULL);



  //Processing a situation in which a window has failed to being created
  if (window == NULL)
  {
    cout << "Failed to create GLFW window\n" ;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  StopTimer("GlfwInit"); 


  StartTimer();
  //Loading pointers on OpenGl functions, processing error of this operation
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    cout << "Failed to initialize GLAD\n";
    return -1;
  }
      
  StopTimer("GladLoad");  
  
  //Editing mouse movements
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPosCallback(window, cursorPositionCallback);


  StartTimer();
  //Vertex shader
  //1)Reading from file
  uint32_t file_sz = 0;
  uint16_t descriptor = open("../Vertex", O_RDONLY);
  char c[1];
  while(read(descriptor, c, 1) > 0)
    file_sz++; 	
  descriptor = open ("../Vertex", O_RDONLY);
  char* VertexShaderSource = new char[file_sz + 1];
  VertexShaderSource[file_sz] = '\0';
  read(descriptor, VertexShaderSource, file_sz);
  //2)Compiling
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
  glCompileShader(vertexShader); 
  //3)Checking successfull compiling
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    cout << "ERROR: vertex shader not compiled\n" << infoLog << "\n";
  }
  StopTimer("VertexLoad"); 


  StartTimer();
  //Fragment shader
  //1)Reading from file 
  file_sz = 0;
  descriptor = open ("../Fragment", O_RDONLY);
  while(read(descriptor, c, 1) > 0)
    file_sz++;  	
  descriptor = open ("../Fragment", O_RDONLY);
  char* fragmentShaderSource = new char[file_sz + 1];
  fragmentShaderSource[file_sz + 1] = '\0';
  read(descriptor, fragmentShaderSource, file_sz);  
  //2)Compiling
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  //3)Checking compilation correctness
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    cout << "ERROR: fragment shader not compiled\n" << infoLog << "\n";
  }
  StopTimer("FragmentLoad");
  
  
  StartTimer(); 
  //binding shaders into a shader program
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram); 
  //Checking linking correctness
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    cout << "ERROR: shader program not linked\n" << infoLog << "\n";
  }
  glDetachShader(shaderProgram, vertexShader);
  glDetachShader(shaderProgram, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader); 
  delete [] fragmentShaderSource;
  delete [] VertexShaderSource;
 
 
  StopTimer("linking shaders");
  float vertices[] =
  {
    1.0f, 1.0f, 0.0f,  
    1.0f, -1.0f, 0.0f,  
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f      
  };
  uint indices[] =
  {
	0, 1, 3,  //first triangle
	1, 2, 3   //second triangle
  };
  
  
  StartTimer();
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO); 
  glBindBuffer(GL_ARRAY_BUFFER, VBO); 
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
   
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  StopTimer("Buffer creation");

  StartTimer();
  glUseProgram(shaderProgram);
  GLint eye_Location = glGetUniformLocation(shaderProgram, "eye");
  GLint lbp_Location = glGetUniformLocation(shaderProgram, "lbp");
  GLint rbp_Location= glGetUniformLocation(shaderProgram, "rbp");
  GLint ltp_Location = glGetUniformLocation(shaderProgram, "ltp");
  GLint rtp_Location = glGetUniformLocation(shaderProgram, "rtp");
  StopTimer("Getting uniform locations");    
      
  //render cicle
  while (!glfwWindowShouldClose(window))
  {
    StartTimer();
    processInput(window);

    //changing color
    glClearColor(0.2f, 0.7f, 0.5f, 0.5f);   //context change function
    glClear(GL_COLOR_BUFFER_BIT);           //the function that applies changes
 
    //drawing rectangle(two triangles)    
    glBindVertexArray(VAO); 
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    //front & back buffer contents interchange
    glfwSwapBuffers(window);
    
    //Checking that the eye is within the confines of the scene
    Check_Confines();
    //Passing eye && projector coordinates to fragment shader
    glUniform3f(eye_Location, eye[0], eye[1], eye[2]);
    glUniform3f(lbp_Location, lbp[0], lbp[1], lbp[2]);
    glUniform3f(rbp_Location, rbp[0], rbp[1], rbp[2]);
    glUniform3f(ltp_Location, ltp[0], ltp[1], ltp[2]);
    glUniform3f(rtp_Location, rtp[0], rtp[1], rtp[2]);
    StopTimerFPS();    
    //This function processes only those events that are already in the event
    //queue and then returns immediately. Processing events will cause the
    //window and input callbacks associated with those events to be called.
    glfwPollEvents();
	  update_coordinates();
	  glfwSetCursorPos(window, HALF_SCR_WIDTH, HALF_SCR_HEIGHT);
    StopTimerFPS();
  }
   
  //glfw: terminating, freeing all allocated resources
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  
  
  cout << "\nAverage FPS:\n" << total_measures / total_time << "\n";
  return 0;
}
