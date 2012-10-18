#include "Utils.h"


void Utils::copy(float* ary, float* src) {
   memcpy(ary, src, sizeof(float)*4);
}  
void Utils::copy(float* ary, float r,  float g, float b, float a) {
   ary[0]=r; ary[1]=g; ary[2]=b; ary[3]=a;
}
void Utils::copy(float* ary, float r,  float g, float b) {
   ary[0]=r; ary[1]=g; ary[2]=b;
}

