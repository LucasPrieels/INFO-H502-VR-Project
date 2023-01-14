#ifndef CUBE_H
#define CUBE_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Mirror.h"

class Cube{
public:
    int x, y, z; // Coordinates of cube
    int texture_ID; // Texture to use for the block
    std::vector<Mirror> mirrors; // List of mirrors attached to this block (so that when the block is destroyed the mirrors are as well)

    static inline std::vector<float> vertices = {
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            // Front face (z = 0.5)
            -0.5f, -0.5f, 0.5f,     0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,      0.0f, 0.5f,     0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,      0.5f, 1.0f,     0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f,       0.5f, 0.5f,     0.0f, 0.0f, 1.0f,
            // Right face (x = 0.5)
            0.5f, -0.5f, 0.5f,      0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       0.0f, 0.5f,     1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.5f, 1.0f,     1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      0.5f, 0.5f,     1.0f, 0.0f, 0.0f,
            // Back face (z = -0.5)
            0.5f, -0.5f, -0.5f,     0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f,      0.0f, 0.5f,     0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,    0.5f, 1.0f,     0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f,     0.5f, 0.5f,     0.0f, 0.0f, -1.0f,
            // Left face (x = -0.5)
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,     0.0f, 0.5f,     -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.5f, 1.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,      0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,
            // Up face (y = 0.5)
            -0.5f, 0.5f, 0.5f,      0.5f, 1.0f,     0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,     0.5f, 0.5f,     0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,      1.0f, 0.5f,     0.0f, 1.0f, 0.0f,
            // Down face (y = -0.5)
            -0.5f, -0.5f, -0.5f,    0.0f, 0.5f,     0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.5f,     0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.5f, 0.0f,     0.0f, -1.0f, 0.0f
    };

    static inline std::vector<unsigned int> vertices_indices = {
            2, 1, 0,
            1, 2, 3,
            6, 5, 4,
            5, 6, 7,
            10, 9, 8,
            9, 10, 11,
            14, 13, 12,
            13, 14, 15,
            18, 17, 16,
            17, 18, 19,
            22, 21, 20,
            21, 22, 23
    };

    Cube(int x, int y, int z, int texture_ID){ // Constructor of Cube takes its coordinates as input
        this->x = x;
        this->y = y;
        this->z = z;

        this->texture_ID = texture_ID;
    }

    void destroy_mirrors_cube(){
        // Destruct the textures linked to the mirrors attached to this cube, to avoid spending time computing views for mirrors that don't exist anymore
        for (Mirror mirror: mirrors){
            for (int i = 0; i < Texture::textures.size(); i++) {
                if (Texture::textures[i].texture_ID == mirror.texture.texture_ID) {
                    for (int j = 0; j < Mirror::mirrors.size(); j++) {
                        if (mirror.texture.texture_ID == Mirror::mirrors[j].texture.texture_ID) {
                            Mirror::mirrors.erase(Mirror::mirrors.begin() + j);

                            j--; // After removing a mirror they are all shifted one position before, so we decrement j to avoid missing one
                        }
                    }
                    Texture::textures.erase(Texture::textures.begin() + i);
                    i--;
                }
            }
        }
    }

    bool part(glm::vec3 pos){ // Checks if position "pos" is inside the cube, meaning it's less than half a cube away from the center of the cube
        return abs(this->x - pos.x) <= 0.51 && abs(this->y - pos.y) <= 0.51 && abs(this->z - pos.z) <= 0.51;
    }

    bool valid_camera_position(glm::vec3 pos){ // Checks if camera position "pos" is too close to this cube
        return abs(this->x - pos.x) <= 0.8 && pos.y - this->y <= 2 && this->y - pos.y <= 0.8 && abs(this->z - pos.z) <= 0.8;
        // The user is supposed to be 2 blocks tall and 1 block wide so the middle of the user should be more than 1 block
        // away from the middle of any block in x, y-, and z direction, 2 in y+ direction (since he is 2 blocks tall)
    }
};
#endif


