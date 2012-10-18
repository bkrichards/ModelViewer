#include "Camera.h"
#include "Screen.h"
#include <stdlib.h>
#include <GL/glut.h>

Camera::Camera()
{
   fovy=90;
   fovyhalf=fovy/2;
   eye[0]=0; eye[1]=0; eye[2]=-2;
   center[0]=0; center[1]=0; center[2]=1;
   up[0]=0; up[1]=1; up[2]=0;
}
Camera::~Camera(){
}
void Camera::SetPerspective()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //cast to float for proper division
   float aspect = (float)Screen::Height/(float)Screen::Width;
   gluPerspective(fovy, aspect, 1, 999);
   //reset matrix mode to calls after this are not affecting the wrong matrix
   glMatrixMode(GL_MODELVIEW);
}

void Camera::SetViewPort()
{
   glViewport(0, 0, Screen::Width, Screen::Height);
}

void Camera::LookAt()
{
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity ();
   gluLookAt(eye[0], eye[1], eye[2],
             center[0], center[1], center[2],
             up[0], up[1], up[2]);
}