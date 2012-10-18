#pragma once
#ifndef __GEOMETRY_UTILS__
#define __GEOMETRY_UTILS__
#include <algorithm>
using namespace std;


#define EPSILON 0.000001 
#define CROSS(dest,v1,v2) \
   dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
   dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
   dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]) 

#define SUB(dest,v1,v2) \
   dest[0]=v1[0]-v2[0]; \
   dest[1]=v1[1]-v2[1]; \
   dest[2]=v1[2]-v2[2];

#define ADD(dest,v1,v2) \
   dest[0]=v1[0]+v2[0]; \
   dest[1]=v1[1]+v2[1]; \
   dest[2]=v1[2]+v2[2];

#define MULT(dest,v1,v2) \
   dest[0]=v1[0]*v2[0]; \
   dest[1]=v1[1]*v2[1]; \
   dest[2]=v1[2]*v2[2];

#define SCALE(v,x) \
   v[0]=v[0]*x; \
   v[1]=v[1]*x;\
   v[2]=v[2]*x;

#define DIV(dest,v1,x) \
   dest[0]=v1[0]/x; \
   dest[1]=v1[1]/x; \
   dest[2]=v1[2]/x;

#define VOP(dest,v1, op, v2) \
   dest[0]=v1[0] op v2[0]; \
   dest[1]=v1[1] op v2[1]; \
   dest[2]=v1[2] op v2[2];

#define COPY(dest, src) \
   dest[0]=src[0]; \
   dest[1]=src[1];\
   dest[2]=src[2];

#define SUM3(v) (v[0]+v[1]+v[2])

#define LENGTH(v) (sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))

#define INIT(v,x) (v[0]=v[1]=v[2]=x)

#define SATURATE(x) (max(0.0f, min(1.0f, x)))

int intersect_triangle(double  orig[3],   double  dir[3],
                       double vert0[3],   double vert1[3], 
                       double vert2[3], 
                       double &t, double &u, double &v);

int intersect_triangle(float orig[3], float dir[3],
                       float vert0[3], float vert1[3], 
                       float vert2[3], 
                       float &t, float &u, float &v);
int intersect_triangle_no_cull(double  orig[3],   double  dir[3],
                       double vert0[3],   double vert1[3], 
                       double vert2[3], 
                       double &t, double &u, double &v);

int intersect_triangle_no_cull(float  orig[3],   float  dir[3],
                       float vert0[3],   float vert1[3], 
                       float vert2[3], 
                       float &t, float &u, float &v);

#endif