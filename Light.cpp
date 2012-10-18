#include "Light.h"
#include "Geometry.h"
#include <algorithm>

Light::Light(int light) {      nLight = light;   }

Light::Light(int light, float parms[][4]) {
   nLight = light;
   copy(MKa, parms[0]);     copy(Loc, parms[1]);
   copy(Ka, parms[2]);      copy(Kd, parms[3]);
   copy(Ks, parms[4]);
}

Light:: ~Light(void) {}

void Light::TurnOn() { glEnable(nLight); }
void Light::TurnOff(){ glDisable(nLight); }

void Light::Draw() {
   //set light model parameters
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MKa);     
   glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
   //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

   //Draw Light
   glLightfv(nLight, GL_POSITION, Loc);
   glLightfv(nLight, GL_AMBIENT, Ka);
   glLightfv(nLight, GL_DIFFUSE, Kd);
   glLightfv(nLight, GL_SPECULAR, Ks);
}

void Light::SetMka(float* src){ copy(MKa, src); }
void Light::SetLoc(float* src){ copy(Loc, src); }
void Light::SetKa(float* src) { copy(Ka, src); }
void Light::SetKd(float* src) { copy(Kd, src); }
void Light::SetKs(float* src) { copy(Ks, src); }

void Light::Enable() { glEnable(GL_LIGHTING); } 
void Light::Disable() { glDisable(GL_LIGHTING); } 
