#ifndef __MATERIAL__
#define __MATERIAL__

#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "Utils.h"
#include "Colors.h"

using namespace Utils;

class Material
{
public:
   Material() {   }

   Material(float se, float parms[][4]) {
      Se = se;
      copy(Ka, parms[0]); copy(Kd, parms[1]);      
      copy(Ks, parms[2]); copy(Ke, parms[3]);
   }

   ~Material(void) {}

   void Draw() {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ka);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Kd);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Ke);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Se);
   }

   //void SetKa(float* src) { init4(Ka, src); }
   void SetKa(float r, float g, float b, float a) { copy(Ka,r,g,b,a); }
   void SetKd(float* src) { copy(Kd, src); }
   void SetKs(float* src) { copy(Ks, src); }
   void SetKe(float* src) { copy(Ke, src); }
   float Ka[4], Kd[4], Ks[4], Ke[3], Se;
   
private:
};

#endif //__MATERIAL__
