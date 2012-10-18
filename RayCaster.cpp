#include <ppl.h>
#include <sstream>
#include <ctime>
#include "RayCaster.h"
#include "Screen.h"
#include "Geometry.h"
#include "Colors.h"

using std::ostringstream;
#define T(x) (model->triangles[(x)])

using namespace Concurrency;

Pixel::Pixel()
{
   draw = false;
   hit[0] = hit[1] = hit[2] = 0;
   //initialize to gray
   r=g=b=.5;
   a=1;
}

Ray::Ray():pixel()
{
   for (int i = 0; i < 3; i++)
      pt[i]=dir[i]=0;
}

void Pixel::SetColor(float* c)
{
   r=SATURATE(c[0]); g=SATURATE(c[1]); b=SATURATE(c[2]); a=1;
}
void Pixel::Draw()
{
   if(!draw) return;

   glColor4f(r,g,b,a);
   glVertex3fv(hit);
}

RayCaster::RayCaster(World* world, Camera* camera)
   :world(world), camera(camera), ch(0), cw(0),rays(0)
{
   parallel=true;
   backfaceCulling=true;
   flat = false;
   rendering=false;
   renderTime = 0.0;
   testShadows = true;
   blinnPhong = false;
   useDistance = false;
   shading = Phong;
   lights = world->Lights;
   Invalidate();
}

RayCaster::~RayCaster(void)
{
   world = nullptr;
}


void RayCaster::Render()
{
   if(!rendering){
      rendering = true;
      if(ch==Screen::Height && cw == Screen::Width && !invalidated) {
         renderPixels();
      } else {
         invalidated = false;
         preallocPixels();
         clock_t start = clock();
         castRays();
         clock_t end = clock();
         renderTime = (double)(end - start)/(double)CLOCKS_PER_SEC;
         renderPixels();   
      }
      setWindowTitle();
      rendering = false;
   }
}
bool allocating = false;
void RayCaster::preallocPixels()
{
   //pixels alreay allocated
   if(ch==Screen::Height && cw == Screen::Width)
      return;
   float dheight, dwidth;
   freepixels();
   ch = Screen::Height;
   cw = Screen::Width;
   rays = new Ray[ch*cw]();
   dheight = 2.0f/(float)ch;
   dwidth = 2.0f/(float)cw;
   if(!parallel){
      for (int i = 0; i < ch*cw; i++)
      {
         int x=i/ch;
         int y=i-(x*ch);
         initRay(&rays[i],x,y, dheight, dwidth);
      }
   } else {
      parallel_for(0, ch*cw, [this, &dheight, &dwidth](int i) 
      {
         int x=i/ch;
         int y=i-(x*ch);
         initRay(&this->rays[i],x,y,dheight,dwidth);
      });
   }
}

void RayCaster::initRay(Ray* ray, int x, int y, float dheight, float dwidth)
{   
   ray->pt[0]=dwidth*x-1.0f;
   ray->pt[1]=dheight*y-1.0f;
   ray->pt[2]=-1;//TODO: get actual z
   SUB(ray->dir,ray->pt, camera->eye);
   glmNormalize(ray->dir);   
}
void RayCaster::castRays_thread(Ray* ray)
{
   GLMtriangle* tri=NULL;
   GLMmaterial* material;
   GLMmodel* model;
   float tuv[3];
   getClosestTriangle(ray, model, tri, material, tuv);

   if(tri != NULL){
      ray->pixel.draw=true;
      calcIntersectionPoint(model, ray, tri, tuv);
      if(material!=NULL)
      {
         float norm[3];
         norm[0]= model->facetnorms[3*tri->findex+0];
         norm[1]= model->facetnorms[3*tri->findex+1];
         norm[2]= model->facetnorms[3*tri->findex+2];
         float facetNorm[3];
         COPY(facetNorm, norm);

         if(!flat) {
            float* N[3]=
            {
               &model->normals[3*tri->nindices[0]],
               &model->normals[3*tri->nindices[1]],
               &model->normals[3*tri->nindices[2]]
            };
            getInterpolatedNorm(N[0], N[1], N[2], tuv, norm);
         }

         float color[3];
         GetPixelColor(material, tri, ray->pixel.hit, ray->dir, norm, facetNorm, color);
         ray->pixel.SetColor(color);
      }
   } else {
      ray->pixel.draw = false;
   }
}
void RayCaster::castRays()
{
   cullTriangles();
   //assumes rays have been allocated
   int num = ch*cw;
   if(!parallel){
      for (int i = 0; i < num; i++){
         int x=i/ch;
         int y=i-(x*ch);
         castRays_thread(&rays[i]);
      }
   } else {
      parallel_for(0,num,[this](int i){
         castRays_thread(&rays[i]);
      });
   }
}
void RayCaster::calcIntersectionPoint(GLMmodel*model,  Ray* ray, GLMtriangle* tri, float* tuv)
{
   float* V[3]=
   {
      &model->vertices[3*tri->vindices[0]],
      &model->vertices[3*tri->vindices[1]],
      &model->vertices[3*tri->vindices[2]]
   };
   float x = 1 - tuv[1] - tuv[2];
   float y = tuv[1];
   float z = tuv[2];
   float t = tuv[0];
   for (int i = 0; i < 3; i++) {
      ray->pixel.hit[i]=ray->pt[i]+t*ray->dir[i];
      //ray->pixel.hit[i] = x*V[0][i]+y*V[1][i]+z*V[2][i];
   }
}
void RayCaster::cullTriangles()
{
   if(!backfaceCulling)
      return;
   float viewNormal[3];
   SUB(viewNormal, camera->center, camera->eye);
   vector<GLMmodel*>* models = world->Models;
   vector<GLMmodel*>::iterator itr = models->begin();
   for (; itr!=end(*models); ++itr){
      GLMmodel* model = *itr;
      if(parallel)
      {
         parallel_for((GLuint)0, model->numtriangles, [this, &model, &viewNormal](GLuint i) 
         {
            cullTriangle_thread(model, viewNormal, i);
         });
      } else {
         for (unsigned int i = 0; i < model->numtriangles; i++)
         {
            cullTriangle_thread(model, viewNormal, i);
         }
      }
   }
}
void RayCaster::cullTriangle_thread(GLMmodel* model, float* viewNormal, unsigned int i)
{
   GLMtriangle* triangle = &model->triangles[i];
   float facetNormal[3] = {
      model->facetnorms[3*triangle->findex+0],
      model->facetnorms[3*triangle->findex+1],
      model->facetnorms[3*triangle->findex+2]};

   float angle = DOT(viewNormal, facetNormal);
   triangle->visible = angle < 0.0000001;
}
bool RayCaster::hitsAnyTriangle( Ray* ray, GLMmodel* model, float* P,GLMtriangle* triOrg, float mindist)
{
   const float MINDIST = .001;//intersection points must be at least this far away.

   float* vertices[3];
   GLuint i;
   GLMgroup* group=model->groups;
   GLMtriangle* tri;
   float tuv[3];
   while (group) 
   {
      for (i = 0; i < group->numtriangles; i++) {
         tri = &T(group->triangles[i]);
         //if(tri == triOrg)//ignore starting triangle
         //   continue;

         vertices[0]=&model->vertices[3 * tri->vindices[0]];
         vertices[1]=&model->vertices[3 * tri->vindices[1]];
         vertices[2]=&model->vertices[3 * tri->vindices[2]];

         if(intersect_triangle_no_cull(ray->pt, ray->dir, vertices[0], vertices[1], vertices[2], tuv[0], tuv[1], tuv[2])) {
            float abst = abs(tuv[0]);
            //test that it's not intersecting with the originating triangle by checking distance
            calcIntersectionPoint(model, ray, tri, tuv);
            if(tuv[2]!=0.0f && tuv[1]!=0.0f && abs(tuv[2]-0.5f)>0.0001 && abs(tuv[1]-0.5f)>0.0001 && abs(1- tuv[2]-tuv[1]) >0.0001f && abs(0.5f- tuv[2]-tuv[1]) > .0001
               && mindist - tuv[0]> 0.00001)
            //if (mindist - tuv[0]> 0.0000001)
               return true;
            else
            {
               continue;
            }
         }
      }

      group = group->next;
   }
   return false;
}

void RayCaster::getClosestTriangle(Ray* ray, GLMmodel* &outModel, GLMtriangle* &outTriangle, GLMmaterial* &outMaterial,float tuv[3])
{
   INIT(tuv, (1<<16));
   float* vertices[3];
   float t,u,v;
   vector<GLMmodel*>* models = world->Models;
   vector<GLMmodel*>::iterator itr = models->begin();
   for (; itr!=end(*models); ++itr){
      GLMmodel* model = *itr;
      GLuint i;
      GLMgroup* group = model->groups;
      GLMtriangle* tri;
      GLMmaterial* material;

      while (group) 
      {
         material = &model->materials[group->material];

         for (i = 0; i < group->numtriangles; i++) {
            tri = &T(group->triangles[i]);

            if(backfaceCulling && !tri->visible)
               continue;//ignore back facing triangles

            vertices[0]=&model->vertices[3 * tri->vindices[0]];
            vertices[1]=&model->vertices[3 * tri->vindices[1]];
            vertices[2]=&model->vertices[3 * tri->vindices[2]];

            if(intersect_triangle(ray->pt, ray->dir, vertices[0], vertices[1], vertices[2], t, u, v)) {
               if(t < tuv[0]) {
                  tuv[0]=t; tuv[1]=u; tuv[2]=v;
                  outTriangle=tri;
                  outMaterial=material;
                  outModel = model;
               }
            }
         }

         group = group->next;
      }
   }
}
void RayCaster::getInterpolatedNorm(float N1[3], float N2[3], float N3[3], float tuv[3], float interNorm[3])
{
   float c = tuv[2];
   float b = tuv[1];
   float a = 1.0f-c-b;
   for (int i = 0; i < 3; i++) {
      interNorm[i]=a*N1[i]+b*N2[i]+c*N3[i];
   }   
}
void RayCaster::renderPixels()
{
   int length = ch*cw;
   glBegin(GL_POINTS);
   for (int i = 0; i < length; i++){
      rays[i].pixel.Draw();
   }
   glEnd();
}
void RayCaster::freepixels()
{
   if(!rays)
      return;
   free(rays);
   rays = NULL;
}
void RayCaster::Invalidate()
{
   invalidated=true;
}
void RayCaster::HandleKey(unsigned char key)
{
   switch (key)
   {
   case 't':
      parallel = !parallel;
      Invalidate();
      break;
   case 'c':
      backfaceCulling=!backfaceCulling;
      Invalidate();
      break;
   case 'f':
      flat = !flat; 
      Invalidate();
      break;
   case 'd':
      useDistance=!useDistance;
      Invalidate();
      break;
   case '1':
      shading = Phong;
      Invalidate();
      break;
   case '2':
      shading = BlinnPhong;
      Invalidate();
      break;
   case 's':
      testShadows = !testShadows;
      Invalidate();
      break;
   }   
}
void RayCaster::setWindowTitle()
{
   ostringstream s;
   s << "Ray:";
   if(parallel)
      s << " Parallel";
   if(backfaceCulling)
      s << " Cull";
   if(flat)
      s<<" Flat";
   if(shading == Phong)
      s<<" Phong";
   else if(shading == BlinnPhong)
      s<<" BlPhong";
   if(useDistance)
      s<<" Dist";
   if(testShadows)
      s<<" Shws";
   s<<": "<<renderTime;
   glutSetWindowTitle(s.str().c_str());   
}

bool RayCaster::inshadow( float* L, float* P, GLMtriangle* tri)
{
   if(!testShadows)
      return false;

   Ray ray;
   COPY(ray.pt, L);
   SUB(ray.dir, P, L);
   float mindist = LENGTH(ray.dir);
   glmNormalize(ray.dir);
   vector<GLMmodel*>* models = world->Models;
   vector<GLMmodel*>::iterator model = models->begin();
   for (; model!=end(*models); ++model){
      if(hitsAnyTriangle(&ray, *model, P, tri, mindist))
         return true;
   }

   return false;
}
void RayCaster::phongShade(Light* light, GLMmaterial* material, float* Ka, float* Kd, float* Ks, float P[3], float V[3], float N[3], float color[3], GLMtriangle* tri)
{
   float L[3], R[3];

   //ambient
   MULT(Ka, light->Ka, material->ambient);

   if(!inshadow(light->Loc, P, tri)){
      //diffuse
      SUB(L,light->Loc,P); //calculate light vector
      float NL = SATURATE(DOT(N,L));
      //Ld*Kd*N.L
      MULT(Kd,light->Kd, material->diffuse);
      if(useDistance)   { 
         float dist = LENGTH(L)*LENGTH(L);
         SCALE(Kd,NL/dist);
      }
      else { 
         SCALE(Kd,NL);
      }

      //specular 
      COPY(R, N);
      SCALE(R, 2.0f);
      SCALE(R, glmDot(N, L));
      SUB(R, R, L);
      glmNormalize(R);
      float RV=SATURATE(DOT(R,V));
      MULT(Ks,light->Ks, material->specular);
      //glm converts ns to opegl compatible reverse this
      SCALE(Ks, pow(RV, material->shininess_raw));
   }
}
void RayCaster::blinnPhongShade(Light* light, GLMmaterial* material, float* Ka, float* Kd, float* Ks, float P[3], float V[3], float N[3], float color[3],GLMtriangle* tri)
{
   float L[3], H[3];

   //ambient
   MULT(Ka, light->Ka, material->ambient);
   if(!inshadow(L, P, tri)){
      //diffuse
      SUB(L,light->Loc,P); //calculate light vector
      glmNormalize(L);
      float NL = SATURATE(DOT(N,L));
      MULT(Kd,light->Kd, material->diffuse);
      SCALE(Kd,NL);

      //specular 
      ADD(H, V, L);//V+L
      glmNormalize(H);
      float HN = SATURATE(DOT(H,N));
      MULT(Ks,light->Ks, material->specular);
      //glm converts ns to opegl compatible reverse this
      SCALE(Ks, pow(HN, material->shininess_raw));
   }
}

void RayCaster::GetPixelColor(GLMmaterial* material, GLMtriangle* tri, float P[3], float V2[3], float N[3], float SN[3], float color[3])
{
   float Ka[3], Kd[3], Ks[3];

   float V[3];
   COPY(V, V2);
   SCALE(V, -1.0f);//reverse view
   //float H[3];

   INIT(color,0);

   ADD(color, color, material->emmissive);

   std::vector<Light*>::iterator light;
   for ( light = lights->begin(); light != lights->end(); ++light ){
      INIT(Ka,0);
      INIT(Kd,0);
      INIT(Ks,0);

      if(shading==Phong) {
         phongShade(*light, material, Ka, Kd, Ks, P, V, N, color, tri);
      } else if(shading == BlinnPhong) {
         blinnPhongShade(*light, material, Ka, Kd, Ks, P, V, N, color, tri);
      }

      ADD(color, color, Ka);
      ADD(color, color, Kd);
      ADD(color, color, Ks);
   }
}
