#pragma once
#ifndef __CAMERA_PARAMS__
#define __CAMERA_PARAMS__

class Camera
{
public:
   Camera();
   ~Camera();
   void SetPerspective();
   void SetViewPort();
   void LookAt();
   double fovy;
   double fovyhalf;
   float eye[3];
   float center[3];
   float up[3];
};

#endif