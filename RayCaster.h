#pragma once

#ifndef __RAY_CASTER__
#define __RAY_CASTER__

#include "World.h"
#include "Camera.h"
#include "Light.h"

typedef enum { Phong=0, BlinnPhong=1 } Shading;

struct Pixel 
{
   bool draw;
   /* calculated point coordinates*/
   float hit[3];
   float u,v,t;
   /* pixel color */
   float r,g,b,a;
   Pixel();
   void SetColor(float* c);
   void Draw();
};

struct Ray
{ 
   /* ray values */
   /* origin */
   float pt[3];
   /* direction */
   float dir[3];
   Pixel pixel;
   Ray();
};


class RayCaster
{
public:
   RayCaster(World* model, Camera* camera);
   ~RayCaster(void);

   bool parallel;
   bool backfaceCulling;
   bool flat;
   void Render();
   void Invalidate();
   void HandleKey(unsigned char key);

private:
   void preallocPixels();
   void freepixels();
   void initRay(Ray* ray, int x, int y, float dheight, float dwidth);
   void calcIntersectionPoint(GLMmodel*model,  Ray* ray, GLMtriangle* tri, float* tuv);
   void castRays();
   void castRays_thread(Ray* ray);
   void getClosestTriangle(Ray* ray, GLMmodel* &model, GLMtriangle* &outTriangle, GLMmaterial* &outMaterial,float tuv[3]);
   bool hitsAnyTriangle(Ray* ray, GLMmodel* model, float* P,GLMtriangle* tri, float mindist);
   void getInterpolatedNorm(float N1[3], float N2[3], float N3[3], float tuv[3], float interNorm[3]);
   void cullTriangles();
   void cullTriangle_thread(GLMmodel* model, float* viewNormal, unsigned int i);
   void renderPixels();
   void setWindowTitle();
   bool inshadow(float* L, float* P,GLMtriangle* tri);
   World* world;
   vector<Light*>* lights;
   Camera* camera;
   Ray* rays;   
   int ch; //current height   
   int cw; //current width
   bool testShadows;
   bool blinnPhong;

   bool rendering;
   bool invalidated;
   double renderTime;

   bool useDistance;
   Shading shading;
   void phongShade(Light* light, GLMmaterial* material, float* Ka, float* Kd, float* Ks, float P[3], float V[3], float N[3], float color[3],GLMtriangle* tri);
   void blinnPhongShade(Light* light, GLMmaterial* material, float* Ka, float* Kd, float* Ks, float P[3], float V[3], float N[3], float color[3],GLMtriangle* tri);
   void GetPixelColor(
                   GLMmaterial* material, /* material light information */
                   GLMtriangle* tri,
                   float P[3], /* ray triangle intersection point */
                   float V[3], /* view ray */
                   float N[3], /* interpolated facet normal */
                   float SN[3], /* facet norm */
                   float color[3] /* out put color*/
                   );
 
};

#endif
