# Minecraft implementation in OpenGL

This aim of this project is to reproduce a simplified version of the Minecraft video game. For that, an immersive environment is rendered, where the user can move, as well as add and destroy different types of blocks. To improve the look of the game, some additional functionalities have been implemented:
- User interactions: control of the camera with keys ZQSD in AZERTY layout, break blocks with left click and create with right click
- Different types of blocks: click on numbers on the keyboard to place blocks with different textures. 1 for grass, 2 for dirt, 3 for gold, 4 for spruce, 5 for bookshelf, 6 for leaves and 7 for glass
- Cubemap which is blue in sunny weather, grey in rainy weather, and black with stars during the night. It rotates around the user to show starts rotating
- Ambient, diffusive, and specular lights. Specular lights depend on the material
- Automatically generated map with trees at startup
- Computing time optimisations with mimapping, face culling, and instanciation
- Sun as light source, rotating around the user to show time passing. The sun becomes orange when close to the horizon to simulate sunrise/sunset
- Transparent objects with blending of objects behind
- Mirrors placed with key 0 of the keyboard, with variable reflection vectors and field of view depending on the camera position
- Mirror borders shown with stencil testing
- Rain with particle system. To toggle the rain between sunny and rainy, click on Enter key
- Shadows: computed from the sun point of view
- Model import and animations: to be implemented

## Repo structure
Libraries are found in the 3rdParty folder (assimp, glm, glfw, stb, and glad). The source code is in folder Project, just like shaders and textures.

## Building and running

The code can easily be built using cmake, as in the following example:

```
mkdir cmake-build
cd cmake-build
cmake ..
make
```

The build takes around 3-4 minutes because of the assimp library. To run the code:

```
cd Project
./Project_v1
```

