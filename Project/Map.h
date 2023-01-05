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

class Map: public Drawable{
public:
    Map(int num_cubes_side, std::string path_to_current_folder):
        Drawable(Cube::vertices, true, Cube::vertices_indices,{3, 2}),
        shader(path_to_current_folder + "vertex_shader_texture.txt", path_to_current_folder + "fragment_shader_texture.txt"),
        texture(path_to_current_folder + "Textures/texture.jpg")
    { // We will create a map of size num_cubes_side x num_cubes_side cubes, with variable altitude

        init_map(num_cubes_side); // Init cubes vector

        // Init texture and shader
        shader.use();
        shader.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0 so we write as uniform
    }

    void draw_cubes(glm::mat4 view, glm::mat4 projection){
        draw(models, view, projection, shader, texture.texture_ID, 36, GL_TRIANGLES);
    }

    void check_remove_cube(glm::vec3 pos) { // Check if the clicked position "pos" corresponds to a cube to remove
        for (int index = 0; index < cubes.size(); index++) {
            if (cubes[index].part(pos)){ // Check if position is less than half a block away from center of cube
                cubes.erase(cubes.begin() + index);
                models.erase(models.begin() + index);
            }
        }
    }

    void check_add_cube(glm::vec3 pos) { // Add the cube corresponding to clicked position "pos"
        std::cout << "Position to add: " << glm::to_string(pos) << std::endl;
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
                Cube new_cube = Cube(x_new_cube, y_new_cube, z_new_cube);
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
    Texture texture; // Texture of blocks

    void init_map(int num_cubes_side){ // Inits a map with cubes
        for (int i = -num_cubes_side/2; i < num_cubes_side/2; i++){
            for (int j = -num_cubes_side/2; j < num_cubes_side/2; j++){
                // Custom altitude function
                int altitude = round(cos((2*(float)i+15)/num_cubes_side*M_PI)+cos(((float)j+12)/num_cubes_side*4*M_PI)+3);

                for (int k = 0; k <= altitude; k++){
                    Cube cube(i, k, j); // Create a new cube at this position, the altitude being on the y-axis
                    cubes.push_back(cube);
                    models.push_back(cube.model); // Add cube model (4x4 matrix) to the list of models
                }
            }
        }
    }
};
#endif