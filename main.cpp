//
//    CSC562 Program 2 - Model Viewer
//    Brian Richards - bkrichar
//    Derived from Rotation sample posted on the message board
#define APP_NAME  "CSC562 Model Viewer"
#define X_AXIS    0
#define Y_AXIS    1
#define Z_AXIS    2

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <Windows.h>
#include <stdlib.h>
#include <GL/GLU.h>

#include "Light.h"
#include "Material.h"
#include "World.h"
#include "Screen.h"
#include "Camera.h"
#include "RayCaster.h"
#include <GL/glut.h>


using namespace std;
using std::ostringstream;

typedef enum {  UP = 1, DOWN, } MouseButtons;/* enumerations for the mouse buttons */
typedef enum { OpenGL=0, RayCast=1 } RenderMode;

RenderMode mode = OpenGL;

int WinId;
int oldX = -13;/* old position of the mouse */
int oldY = -13;
int mState = UP;/* mouse state, UP or DOWN */
int axisRot = Z_AXIS;/* current axis of rotation */
float rotate = 0.0f;/* amount to rotate about axis */
float axis[] = {0.0, 0.0, 1.0};/* vector which describes the axis to rotate about */
float gRot[]={0,0,0};/* global rotation, for use with the mouse */

World* world;
Camera* camera;
RayCaster* rayCaster;

//function headers
void drawOpenGL();
void drawRayCaster();
void resetRotation() { gRot[0] = gRot[1] = gRot[2] = 0; }

/*Idle function, makes it animate.*/
void glutIdle(void)
{
}
/*Resize function, called on app start and on resize.*/
void glutResize(int w, int h)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   Screen::Width = w;
   Screen::Height = h;

   camera->SetPerspective();
   camera->SetViewPort();

   glutPostRedisplay();
}

/*Keyboard handler.*/
void glutKeyboard(unsigned char key, int x, int y)
{
   switch(key)
   {
   case ' ':
      mode = (RenderMode)!mode;
      break;
   case 'r':
      resetRotation();
      break;
   }
   rayCaster->HandleKey(key);
   glutPostRedisplay();
}
/*Clamps the rotation to -360 to 360*/
void clamp(float* v)
{
   int i;

   for(i = 0; i < 3; i ++)
      if(v[i] > 360 || v[i] < -360)
         v[i] = 0.0f;
}
/*Called when the mouse moves in our app area.*/
void glutMotion(int x, int y)
{
   if(mState == DOWN)
   {
      gRot[0] -=((oldY - y) * 180.0f) / 100.0f;
      gRot[1] -=((oldX - x) * 180.0f) / 100.0f;
   }
   clamp(gRot);
   glutPostRedisplay();
   oldX = x;
   oldY = y;
}

/*Called when the mouse is clicked.*/
void glutMouse(int button, int state, int x, int y)
{
   if(state == GLUT_DOWN)
   {
      switch(button)
      {
      case GLUT_LEFT_BUTTON:
      case GLUT_RIGHT_BUTTON:
         mState = DOWN;
         oldX = x;
         oldY = y;
         break;
      }
   } else if(state == GLUT_UP)
      mState = UP;
   glutPostRedisplay();
}
/*Sets up various OpenGL stuff.*/
void glInit(void)
{
   glEnable(GL_DEPTH_TEST);

   world = new World();
   camera = new Camera();
   rayCaster = new RayCaster(world, camera);
}

float lightDelta=0.25f;
void glutSpecial(int key, int x, int y)
{
   if(key == GLUT_KEY_RIGHT)     { world->NextModel(); rayCaster->Invalidate(); }
   else if(key ==GLUT_KEY_LEFT)  { world->PrevModel(); rayCaster->Invalidate(); }
   else if(key == GLUT_KEY_UP)   { camera->fovy -= 0.5; camera->SetPerspective(); }
   else if(key == GLUT_KEY_DOWN) { camera->fovy += 0.5; camera->SetPerspective(); }
   glutPostRedisplay();
}

/* The display function. */
bool indisplay=false;
void glutDisplay(void)
{  
   if(indisplay)//ray casting can take too long need to prevent reentrance
      return;
   indisplay=true;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   camera->LookAt();

   glClearColor(0.0, 0.0, 0.0, 0.0);   
   glPushMatrix();
   glRotatef(gRot[0], 1.0, 0.0, 0.0);
   glRotatef(gRot[1], 0.0, 1.0, 0.0);

   if (mode == OpenGL){
      drawOpenGL();
   }
   else
   {
      drawRayCaster();
   }

   glPopMatrix();
   glutSwapBuffers();
   indisplay = false;
}

void drawOpenGL()
{
   world->DrawLights();
   world->DrawModels();
   glutSetWindowTitle("OpenGL");
}

void drawRayCaster()
{
   world->DisableLights();
   rayCaster->Render();  
}

int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, char*	lpCmdLine, int nCmdShow)
{   
   FILE* window;

   window = fopen("window.txt", "r");
   if(window)
   {
      float h, w;
      fscanf(window, "%f %f", &h, &w);
      Screen::Height = h;
      Screen::Width = w;
      
      fclose(window);
   }

   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(Screen::Width, Screen::Height);
   WinId=glutCreateWindow(APP_NAME);
   glutKeyboardFunc(glutKeyboard);
   glutDisplayFunc(glutDisplay);
   glutReshapeFunc(glutResize);
   glutMotionFunc(glutMotion);
   glutMouseFunc(glutMouse);
   glutIdleFunc(glutIdle);
   glutSpecialFunc(glutSpecial);
   glInit();
   glutMainLoop();
}
