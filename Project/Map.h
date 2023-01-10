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
#include "Mirror.h"

class Map: public Drawable{
public:
    Map(int num_cubes_side, std::string path_to_current_folder):
        Drawable(Cube::vertices, true, Cube::vertices_indices,{3, 2, 3}),
        shader(path_to_current_folder + "vertex_shader_texture.txt", path_to_current_folder + "fragment_shader_texture.txt")
    { // We will create a map of size num_cubes_side x num_cubes_side cubes, with variable altitude
        this->path_to_current_folder = path_to_current_folder;
        init_map(num_cubes_side); // Init cubes vector
    }

    void draw_opaque_cubes(glm::mat4 view, glm::mat4 projection, Sun sun, glm::vec3 camera_pos){
        shader.use();
        shader.set_uniform("light_color", sun.light_color);
        shader.set_uniform("light_pos", sun.light_pos);
        shader.set_uniform("viewing_pos", camera_pos);
        shader.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0, so we write as uniform

        // First draw only opaque objects (to make sure we see them through non-opaque ones)
        for (Texture texture: Texture::textures) {
            if (!texture.opaque || texture.mirror) continue; // Skip non-opaque and mirror objects
            std::vector<glm::vec3> translations_to_draw;
            for (Cube cube: cubes) { // Look for all cubes having this texture and put them in vector translations_to_draw
                if (cube.texture_ID == texture.texture_ID)
                    translations_to_draw.push_back(glm::vec3(cube.x, cube.y, cube.z));
            }
            shader.set_uniform("shininess", texture.shininess);
            glEnable(GL_CULL_FACE); // Improves computation power and allows to have leaves blocks without flickering
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            draw(translations_to_draw, view, projection, shader, texture.texture_ID, 36, GL_TRIANGLES, true);
            glDisable(GL_CULL_FACE);
        }
    }

    void draw_non_opaque_cubes(glm::mat4 view, glm::mat4 projection, Sun sun, glm::vec3 camera_pos){
        // Then draw non-opaque objects starting with the furthest away
        std::vector<std::pair<float, glm::vec3>> translations_to_draw;
        std::vector<std::pair<float, Texture>> textures_to_draw;
        for (Texture texture: Texture::textures) {
            if (texture.opaque || texture.mirror) continue; // Skip opaque and mirror objects
            for (Cube cube: cubes) { // Look for all cubes having this texture and put them in vector models_to_draw
                if (cube.texture_ID == texture.texture_ID){
                    float distance = glm::length(camera_pos-glm::vec3(cube.x, cube.y, cube.z));
                    translations_to_draw.push_back(std::make_pair(distance, glm::vec3(cube.x, cube.y, cube.z)));
                    textures_to_draw.push_back(std::make_pair(distance, texture));
                }
            }
        }

        std::sort(translations_to_draw.begin(), translations_to_draw.end(), sort_by_first_val_vec3);
        std::sort(textures_to_draw.begin(), textures_to_draw.end(), sort_by_first_val_texture);

        glEnable(GL_CULL_FACE); // Improves computation power and allows to have leaves blocks without flickering
        glEnable(GL_BLEND); // Allows blending of semi-transparent objects
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int i = translations_to_draw.size()-1; i >= 0; i--){
            shader.set_uniform("shininess", textures_to_draw[i].second.shininess);
            draw({translations_to_draw[i].second}, view, projection, shader, textures_to_draw[i].second.texture_ID, 36, GL_TRIANGLES, true);
        }
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }

    void check_remove_cube(glm::vec3 pos) { // Check if the clicked position "pos" corresponds to a cube to remove
        for (int index = 0; index < cubes.size(); index++) {
            if (cubes[index].part(pos)){ // Check if position is less than half a block away from center of cube
                cubes[index].destroy_mirrors_cube();
                cubes.erase(cubes.begin() + index);
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
                int x_variation = 0, y_variation = 0, z_variation = 0;
                if (pos.x - cubes[index].x > 0.49) x_variation++;
                else if (pos.x - cubes[index].x < -0.49) x_variation--;
                else if (pos.y - cubes[index].y > 0.49) y_variation++;
                else if (pos.y - cubes[index].y < -0.49) y_variation--;
                else if (pos.z - cubes[index].z > 0.49) z_variation++;
                else if (pos.z - cubes[index].z < -0.49) z_variation--;
                x_new_cube += x_variation;
                y_new_cube += y_variation;
                z_new_cube += z_variation;

                // texture_num takes the special value -1 when asked for a mirror texture
                if (texture_num == -1){
                    glm::vec3 mirror_position = glm::vec3(x_new_cube, y_new_cube, z_new_cube);
                    glm::vec3 mirror_orientation = glm::vec3(x_variation, y_variation, z_variation); // Mirror faces the opposite direction as where the user clicked to place it
                    std::vector<float> vertices;
                    if (x_variation > 0) vertices = Mirror::vertices_x_plus;
                    else if (x_variation > 0) vertices = Mirror::vertices_x_plus;
                    else if (x_variation < 0) vertices = Mirror::vertices_x_minus;
                    else if (y_variation > 0) vertices = Mirror::vertices_y_plus;
                    else if (y_variation < 0) vertices = Mirror::vertices_y_minus;
                    else if (z_variation > 0) vertices = Mirror::vertices_z_plus;
                    else if (z_variation < 0) vertices = Mirror::vertices_z_minus;
                    cubes[index].mirrors.push_back(Mirror(path_to_current_folder, mirror_position, mirror_orientation, vertices));
                    break; // Since we add a mirror we don't add a cube more
                }
                Cube new_cube = Cube(x_new_cube, y_new_cube, z_new_cube, Texture::textures[texture_num].texture_ID);
                if (new_cube.valid_camera_position(position_camera)) continue; // If the cube is too close to the camera we don't place it, otherwise the camera can't move anymore
                cubes.push_back(new_cube);
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
    Shader shader; // Shader used to draw blocks
    std::string path_to_current_folder;

    void init_map(int num_cubes_side){ // Inits a map with cubes
        for (int i = -num_cubes_side/2; i < num_cubes_side/2; i++){
            for (int j = -num_cubes_side/2; j < num_cubes_side/2; j++){
                // Custom altitude function
                int altitude = round(cos((2*(float)i+15)/80*M_PI)+cos(((float)j+12)/80*4*M_PI)+3);

                for (int k = 0; k < altitude; k++){ // Create dirt blocks until altitude-1
                    Cube cube(i, k, j, Texture::textures[1].texture_ID); // Create a new cube at this position, the altitude being on the y-axis
                    cubes.push_back(cube);
                }
                // Then create a grass block at altitude
                Cube cube(i, altitude, j, Texture::textures[0].texture_ID); // Create a new cube at this position, the altitude being on the y-axis
                cubes.push_back(cube);

                // Add 4 trees on the map at (-7,-12), (-28, 8), (31, -32), and (12, 28) (only if they are in range of the map, i.e. between -num_cubes_side/2 and num_cubes_side/2)
                if ((i == -7 && j == -12) || (i == -28 && j == 8) || (i == 31 && j == -32) || (i == 12 && j == 28)) {
                    // Add 6 spruce blocks on top of each other
                    for (int k = 1; k <= 6; k++) {
                        Cube cube(i, altitude + k, j, Texture::textures[3].texture_ID);
                        cubes.push_back(cube);
                    }

                    // Add leaf blocks: 4 on the altitude+4 level, 8 on the altitude+5 and altitude+6 levels, and 5 on the altitude+7 level
                    for (int k = 4; k <= 7; k++){
                        std::vector<std::pair<int, int>> offsets;
                        if (k == 4) offsets = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                        if (k == 5 || k == 6) offsets = {{-1,0}, {1,0}, {0,-1}, {0,1}, {-1,-1}, {-1,1}, {1,-1}, {1,1}};
                        if (k == 7) offsets = {{-1,0}, {1,0}, {0,-1}, {0,1}, {0,0}};
                        for (std::pair<int, int> offset: offsets) {
                            Cube cube(i + offset.first, altitude + k, j + offset.second, Texture::textures[5].texture_ID);
                            cubes.push_back(cube);
                        }
                    }
                }
            }
        }
    }

    static bool sort_by_first_val_vec3(std::pair<float, glm::vec3> &a, std::pair<float, glm::vec3> &b){
        return (a.first < b.first);
    }

    static bool sort_by_first_val_texture(std::pair<float, Texture> &a, std::pair<float, Texture> &b){
        return (a.first < b.first);
    }
};
#endif