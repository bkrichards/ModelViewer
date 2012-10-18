#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <direct.h>

#include "World.h"
#include "Geometry.h"

using namespace std;

World::World(void)
{
   LoadModels("world.txt");
   LoadLights("lights.txt");
}

World::~World(void)
{
   deleteModels();
}

void World::translateModel(GLMmodel* model, float* translate)
{
    GLuint i;
    
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[3 * i + 0] += translate[0];
        model->vertices[3 * i + 1] += translate[1];
        model->vertices[3 * i + 2] += translate[2];
    }
}
void World::LoadModel(char* file, float scale, float* translate)
{
   GLMmodel* model = glmReadOBJ(file);
   glmScale(model, scale);
   translateModel(model, translate);
   glmFacetNormals(model);
   glmVertexNormals(model, 90);
   models.push_back(model);
}

void World::LoadModels(char* filename)
{
   float scale = 1;
   float translate[3];
   FILE* file;
   char buf[128];

   file = fopen(filename, "r");
   if (!file) {
      return;
   }
   while(fscanf(file, "%s %f %f %f %f", 
      buf,
      &scale,
      &translate[0],&translate[1],&translate[2]) != EOF) {
      LoadModel(buf, scale, translate);
      scale = 1;
      INIT(translate, 0.0f);
   }
   fclose(file);
}

void resetLightParams(float lightParams[5][4])
{
   INIT(lightParams[0],0.0f);
   INIT(lightParams[1],0.0f);
   INIT(lightParams[2],0.0f);
   INIT(lightParams[3],0.0f);
   INIT(lightParams[4],0.0f);
}
void World::LoadLights(char* filename)
{
   const int LOC=1;
   const int KA=2;
   const int KD=3;
   const int KS=4;
   float lightParams[5][4] =
   { 
      {0, 0, 0, 1}, //mka
      {0, 5, 0, 1}, //loc
      {1, 1, 1, 1}, //Ka
      {1, 1, 1, 1}, //Kd
      {1, 1, 1, 1} //Ks
   };

   FILE* file;
   char buf[128];

   file = fopen(filename, "r");
   if (!file) {
      lights.push_back(new Light(GL_LIGHT0, lightParams));
      return;
   }
   bool createLight = false;
   int lightCount=0;
   while(fscanf(file, "%s", buf) != EOF) {
      switch(buf[0]) {
      case '#':               /* comment */
         if(createLight)
            lights.push_back(new Light(GL_LIGHT0+lightCount++, lightParams));
         resetLightParams(lightParams);
         createLight = true;
         /* eat up rest of line */
         fgets(buf, sizeof(buf), file);
         break;
      case 'L':
      case 'l':
         fscanf(file, "%f %f %f",
            &lightParams[LOC][0],
            &lightParams[LOC][1],
            &lightParams[LOC][2]);
         break;
      case 'K':
         switch(buf[1]) {
         case 'd':
            fscanf(file, "%f %f %f",
               &lightParams[KD][0],
               &lightParams[KD][1],
               &lightParams[KD][2]);
            break;
         case 's':
            fscanf(file, "%f %f %f",
               &lightParams[KS][0],
               &lightParams[KS][1],
               &lightParams[KS][2]);
            break;
         case 'a':
            fscanf(file, "%f %f %f",
               &lightParams[KA][0],
               &lightParams[KA][1],
               &lightParams[KA][2]);
            break;
         default:
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
         }
         break;
      default:
         /* eat up rest of line */
         fgets(buf, sizeof(buf), file);
         break;
      }
   }

   if(createLight)
      lights.push_back(new Light(GL_LIGHT0+lightCount++, lightParams));
   fclose(file);
}

void World::Clear()
{
   deleteModels();
}

void World::NextModel()
{
   currModel=(currModel+1) % models.size();
}

void World::PrevModel()
{
   currModel--;
   if(currModel<0)
      currModel = models.size()-1;
}

void World::DrawModels()
{
   vector<GLMmodel*>::iterator model = models.begin();
   for (; model!=end(models); ++model){
      glmDraw(*model, GLM_SMOOTH| GLM_COLOR | GLM_MATERIAL);
   }
}
void World::DrawCurrModel()
{
   if(models.size()>0)
   {
      GLMmodel* model = models.at(currModel);
      glmDraw(model, GLM_SMOOTH| GLM_COLOR | GLM_MATERIAL);
   }
}

void World::DrawLights()
{
   Light::Enable();
   std::vector<Light*>::iterator light;
   for ( light = lights.begin(); light != lights.end(); ++light ){
      (*light)->TurnOn();
      (*light)->Draw();
   }   
}

void World::DisableLights()
{
   Light::Disable();
   std::vector<Light*>::iterator light;
   for ( light = lights.begin(); light != lights.end(); ++light ){
      (*light)->TurnOff();
   }
}

void World::deleteModels()
{
   for_each( models.begin(),
      models.end(),
      DeleteVector<GLMmodel*>());
   models.clear();   
}