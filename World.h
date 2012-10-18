#pragma once
#ifndef __MODEL_MANAGER__
#define __MODEL_MANAGER__

#include <string>
#include <iostream>
#include <vector>

#include "glm\glm.h"
#include "Utils.h"
#include "Light.h"

using namespace std;


class World
{
public:
   World(void);
   ~World(void);
   void LoadModel(char* filename, float scale, float translate[3]);
   void LoadModels(char* filename);
   void LoadLights(char* filename);
   vector<GLMmodel*>* getModels() { return &models; }
   vector<Light*>* getLights() { return &lights; }
   GLMmodel* getCurrModel() { return models.at(currModel); }
   void Clear();   
   void NextModel();
   void PrevModel();
   void DrawCurrModel();
   void DrawModels();
   void DrawLights();
   void DisableLights();
   __declspec(property(get = getModels)) vector<GLMmodel*>* Models;
   __declspec(property(get = getLights)) vector<Light*>* Lights;
   __declspec(property(get = getCurrModel)) GLMmodel* CurrModel;

private:
   void translateModel(GLMmodel* model, float* translate);
   vector<GLMmodel*> models;
   vector<Light*> lights;
   void deleteModels();
   int currModel;
};

#endif