#ifndef __LIGHT__
#define __LIGHT__

#include <stdlib.h>
#include <vector>
#include <math.h>
#include <GL/glut.h>
#include "glm/glm.h"
#include "Utils.h"

using namespace Utils;
using namespace std;

class Light
{
public:
   Light(int light);
   Light(int light, float parms[][4]);
   ~Light(void);
   void TurnOn();
   void TurnOff();
   void Draw();
   void SetMka(float* src);
   void SetLoc(float* src);
   void SetKa(float* src);
   void SetKd(float* src);
   void SetKs(float* src);
   static void Enable();
   static void Disable();
   
   float MKa[4], Loc[4], Ka[4], Kd[4], Ks[4];
  
private:
   int nLight;  
};



#endif //__LIGHT__