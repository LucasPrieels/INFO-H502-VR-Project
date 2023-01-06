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
#include <map>
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
        shader.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0, so we write as uniform

        // First draw only opaque objects (to make sure we see them through non-opaque ones)
        int nb_cubes = 0;
        for (Texture texture: textures){
            if (!texture.opaque) continue; // Skip non-opaque objects
            std::vector<glm::mat4> models_to_draw;
            for (Cube cube: cubes){ // Look for all cubes having this texture and put them in vector models_to_draw
                if (cube.texture_ID == texture.texture_ID) models_to_draw.push_back(cube.model);
            }
            shader.set_uniform("shininess", texture.shininess);
            glEnable(GL_CULL_FACE); // Improves computation power and allows to have leaves blocks without flickering
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            draw(models_to_draw, view, projection, shader, texture.texture_ID, 36, GL_TRIANGLES);
            glDisable(GL_CULL_FACE);
            nb_cubes += models_to_draw.size();
        }

        // Then draw non-opaque objects starting with the furthest away
        std::vector<std::pair<float, glm::mat4>> models_to_draw;
        std::vector<std::pair<float, Texture>> textures_to_draw;
        for (Texture texture: textures) {
            if (texture.opaque) continue; // Skip opaque objects
            for (Cube cube: cubes) { // Look for all cubes having this texture and put them in vector models_to_draw
                if (cube.texture_ID == texture.texture_ID){
                    float distance = glm::length(camera_pos-glm::vec3(cube.x, cube.y, cube.z));
                    models_to_draw.push_back(std::make_pair(distance, cube.model));
                    textures_to_draw.push_back(std::make_pair(distance, texture));
                }
            }
        }

        std::sort(models_to_draw.begin(), models_to_draw.end(), sort_by_first_val_mat4);
        std::sort(textures_to_draw.begin(), textures_to_draw.end(), sort_by_first_val_texture);

        glEnable(GL_CULL_FACE); // Improves computation power and allows to have leaves blocks without flickering
        glEnable(GL_BLEND); // Allows blending of semi-transparent objects
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        std::cout << nb_cubes << " " << models_to_draw.size() << " " << cubes.size() << std::endl;
        for (int i = models_to_draw.size()-1; i >= 0; i--){
            shader.set_uniform("shininess", textures_to_draw[i].second.shininess);
            draw({models_to_draw[i].second}, view, projection, shader, textures_to_draw[i].second.texture_ID, 36, GL_TRIANGLES);
        }
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }

    void check_remove_cube(glm::vec3 pos) { // Check if the clicked position "pos" corresponds to a cube to remove
        for (int index = 0; index < cubes.size(); index++) {
            if (cubes[index].part(pos)){ // Check if position is less than half a block away from center of cube
                cubes.erase(cubes.begin() + index);
                models.erase(models.begin() + index);
            }
        }
    }

    void add_cube(glm::vec3 pos, int texture_num, glm::vec3 position_camera) { // Add the cube corresponding to clicked position "pos"
        for (int index = 0; index < cubes.size(); index++) {
            if (cubes[index].part(pos)){ // Check if position is less than half a block away from center of cube
                // The new cube will be placed alongside cubes[index], but not at the same position, rather just besides it
                int x_new_cube = cubes[index].x;
                int y_new_cube = cubes[index].y;
                int z_new_cube = cubes[index].z;
                // The direction in which the clicked position is the furthest away from cubes[index] is the direction in which we need to increment coordinate of cubes[index]
                if (pos.x - cubes[index].x > 0.49) x_new_cube++;
                else if (pos.x - cubes[index].x < -0.49) x_new_cube--;
                else if (pos.y - cubes[index].y > 0.49) y_new_cube++;
                else if (pos.y - cubes[index].y < -0.49) y_new_cube--;
                else if (pos.z - cubes[index].z > 0.49) z_new_cube++;
                else if (pos.z - cubes[index].z < -0.49) z_new_cube--;
                Cube new_cube = Cube(x_new_cube, y_new_cube, z_new_cube, textures[texture_num].texture_ID);
                if (new_cube.valid_camera_position(position_camera)) continue; // If the cube is too close to the camera we don't place it, otherwise the camera can't move anymore
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

                // Add 4 trees on the map at (-7,-12), (-28, 8), (31, -32), and (12, 28) (only if they are in range of the map, i.e. between -num_cubes_side/2 and num_cubes_side/2)
                if ((i == -7 && j == -12) || (i == -28 && j == 8) || (i == 31 && j == -32) || (i == 12 && j == 28)) {
                    // Add 6 spruce blocks on top of each other
                    for (int k = 1; k <= 6; k++) {
                        Cube cube(i, altitude + k, j, textures[3].texture_ID);
                        cubes.push_back(cube);
                        models.push_back(cube.model); // Add cube model (4x4 matrix) to the list of models
                    }

                    // Add leaf blocks: 4 on the altitude+4 level, 8 on the altitude+5 and altitude+6 levels, and 5 on the altitude+7 level
                    for (int k = 4; k <= 7; k++){
                        std::vector<std::pair<int, int>> offsets;
                        if (k == 4) offsets = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                        if (k == 5 || k == 6) offsets = {{-1,0}, {1,0}, {0,-1}, {0,1}, {-1,-1}, {-1,1}, {1,-1}, {1,1}};
                        if (k == 7) offsets = {{-1,0}, {1,0}, {0,-1}, {0,1}, {0,0}};
                        for (std::pair<int, int> offset: offsets) {
                            Cube cube(i + offset.first, altitude + k, j + offset.second, textures[5].texture_ID);
                            cubes.push_back(cube);
                            models.push_back(cube.model); // Add cube model (4x4 matrix) to the list of models
                        }
                    }
                }
            }
        }
    }

    static bool sort_by_first_val_mat4(std::pair<float, glm::mat4> &a, std::pair<float, glm::mat4> &b){
        return (a.first < b.first);
    }

    static bool sort_by_first_val_texture(std::pair<float, Texture> &a, std::pair<float, Texture> &b){
        return (a.first < b.first);
    }
};
#endif