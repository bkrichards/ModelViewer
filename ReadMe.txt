CSC561 - Fall 2012 - Assignment 2 Ray Caster
Brian Richards - bkrichar
Run Output/Program1.exe

Open ModelViewer.sln
When ray casting the top window will show the different ray cast parameters and the amount of time required to render all the rays, excluding the actual drawing time.


Input:

world.txt:
model file on each line, all values are required. Scale will scale the model by the given factor, translate will move the model, no support for rotation. All the models will be rendered together.
<model file> <scale> <translate.x> <translate.y> <translate.z>

lights.txt: define lights, up to 8, # is necessary to define the start of the light, location, Ka, Kd, and Ks, can be specified. Any missing values will default to 0.
#light1 
loc 0.0 5.0 -1.0 1.0
Ka 0.5 0.5 0.5 1.0
Kd 1.0 1.0 1.0 1.0
Ks 1.0 1.0 1.0 1.0

Keys:
Space: Toggle bettween drawing models with opengl and ray casting
Mouse: rotate models in opengl mode
t: toggle parallel rendering, default is parallel
c: toggle backface culling of triangle prior to ray casting, this will determine all the back facing triangles based on the magnitude of the normal of the ray shot straight from the eye. Eliminating the need to test the back facing triangles for each ray.
f: disables phong shading, uses flat shading
d: attenuates the light based on the square of the distance, no support for attenuation parameters
1: use phong shading, default
2: use blinn-phong shading, some noticable artifacts with shadows here
s: toggle shadow rendering, default is on
   
Extra Credit:
10 Parallelzing
2.5 Backface culling: 
With threading and backface culling the render time on my laptop with 4 cores (8 hyper threads) with my sample world file goes from ~4.6s to ~0.65s, about a seven fold increase in performance
5 Support for arbitrary window sizes, with resize and init with window.txt
5 support arbitrary number of objects with scale, translate, no rotate
5 detect shadows

3rd party help:
Triangle intersection code was from a paper thats in the papers folder "Fast Minimum Storage Ray/Triangle Intersection"
GLM used for loading models.