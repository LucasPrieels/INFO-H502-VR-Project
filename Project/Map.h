#ifndef MAP_H
#define MAP_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Drawable.h"
#include "Texture.h"
#include "Shader.h"
#include "Cube.h"
#include "Sun.h"

class Map: public Drawable{
public:
    Map(int num_cubes_side, std::string path_to_current_folder, std::vector<Texture> textures):
        Drawable(Cube::vertices, true, Cube::vertices_indices,{3, 2, 3}),
        shader(path_to_current_folder + "vertex_shader_texture.txt", path_to_current_folder + "fragment_shader_texture.txt")
    { // We will create a map of size num_cubes_side x num_cubes_side cubes, with variable altitude
        this->path_to_current_folder = path_to_current_folder;
        this->textures = textures;
        init_map(num_cubes_side); // Init cubes vector
    }

    void draw_cubes(glm::mat4 view, glm::mat4 projection, Sun sun, glm::vec3 camera_pos){
        shader.use();
        shader.set_uniform("light_color", sun.light_color);
        shader.set_uniform("light_pos", sun.light_pos);
        shader.set_uniform("viewing_pos", camera_pos);
        for (Texture texture: textures){
            std::vector<glm::mat4> models_to_draw;
            for (Cube cube: cubes){ // Look for all cubes having this texture and put them in vector models_to_draw
                if (cube.texture_ID == texture.texture_ID) models_to_draw.push_back(cube.model);
            }
            shader.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0 so we write as uniform
            shader.set_uniform("shininess", texture.shininess);
            draw(models_to_draw, view, projection, shader, texture.texture_ID, 36, GL_TRIANGLES);
        }
    }

    void check_remove_cube(glm::vec3 pos) { // Check if the clicked position "pos" corresponds to a cube to remove
        for (int index = 0; index < cubes.size(); index++) {
            if (cubes[index].part(pos)){ // Check if position is less than half a block away from center of cube
                cubes.erase(cubes.begin() + index);
                models.erase(models.begin() + index);
            }
        }
    }

    void add_cube(glm::vec3 pos, int texture_num) { // Add the cube corresponding to clicked position "pos"
        for (int index = 0; index < cubes.size(); index++) {
            if (cubes[index].part(pos)){ // Check if position is less than half a block away from center of cube
                // The new cube will be placed alongside cubes[index], but not at the same position, rather just besides it
                int x_new_cube = cubes[index].x;
                int y_new_cube = cubes[index].y;
                int z_new_cube = cubes[index].z;
                // The direction in which the clicked position is the furtest away from cubes[index] is the direction in which we need to increment coordinate of cubes[index]
                if (pos.x - cubes[index].x > 0.49) x_new_cube++;
                else if (pos.x - cubes[index].x < -0.49) x_new_cube--;
                else if (pos.y - cubes[index].y > 0.49) y_new_cube++;
                else if (pos.y - cubes[index].y < -0.49) y_new_cube--;
                else if (pos.z - cubes[index].z > 0.49) z_new_cube++;
                else if (pos.z - cubes[index].z < -0.49) z_new_cube--;
                Cube new_cube = Cube(x_new_cube, y_new_cube, z_new_cube, textures[texture_num].texture_ID);
                cubes.push_back(new_cube);
                models.push_back(new_cube.model);
                break; // If we already added the new cube alongside a cube, we won't place it alongside another one
            }
        }
    }

    bool part_of_cubes(glm::vec3 pos){ // Checks if the given position is too close to any of the cubes
        for (Cube cube: cubes) if (cube.valid_camera_position(pos)) return false;
        return true;
    }

private:
    std::vector<Cube> cubes; // List of the cubes in the map
    std::vector<glm::mat4> models; // List of model matrices of each block
    Shader shader; // Shader used to draw blocks
    std::vector<Texture> textures;
    std::string path_to_current_folder;

    void init_map(int num_cubes_side){ // Inits a map with cubes
        for (int i = -num_cubes_side/2; i < num_cubes_side/2; i++){
            for (int j = -num_cubes_side/2; j < num_cubes_side/2; j++){
                // Custom altitude function
                int altitude = round(cos((2*(float)i+15)/num_cubes_side*M_PI)+cos(((float)j+12)/num_cubes_side*4*M_PI)+3);

                for (int k = 0; k < altitude; k++){ // Create dirt blocks until altitude-1
                    Cube cube(i, k, j, textures[1].texture_ID); // Create a new cube at this position, the altitude being on the y-axis
                    cubes.push_back(cube);
                    models.push_back(cube.model); // Add cube model (4x4 matrix) to the list of models
                }
                // Then create a grass block at altitude
                Cube cube(i, altitude, j, textures[0].texture_ID); // Create a new cube at this position, the altitude being on the y-axis
                cubes.push_back(cube);
                models.push_back(cube.model); // Add cube model (4x4 matrix) to the list of models
            }
        }
    }
};
#endif