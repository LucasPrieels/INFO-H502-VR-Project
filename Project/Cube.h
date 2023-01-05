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
#include "Shader.h"
#include "Texture.h"

class Cube{
public:
    int x, y, z; // Coordinates of cube
    glm::mat4 model; // Model matrix, taking into account its position

    static inline std::vector<float> vertices = {
            -0.5f, 0.5f, 0.5f, 0.25f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.5f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.666f,
            -0.5f, -0.5f, 0.5f, 0.25f, 0.666f,
            0.5f, -0.5f, 0.5f, 0.5f, 0.666f,
            0.5f, 0.5f, 0.5f, 0.75f, 0.666f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.666f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.334f,
            -0.5f, -0.5f, -0.5f, 0.25f, 0.334f,
            0.5f, -0.5f, -0.5f, 0.5f, 0.334f,
            0.5f, 0.5f, -0.5f, 0.75f, 0.334f,
            -0.5f, 0.5f, -0.5f, 1.0f, 0.334f,
            -0.5f, 0.5f, -0.5f, 0.25f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.5f, 0.0f
    };

    static inline std::vector<unsigned int> vertices_indices = {
            0, 1, 3,
            1, 3, 4,
            2, 3, 7,
            3, 7, 8,
            3, 4, 8,
            4, 8, 9,
            4, 5, 9,
            5, 9, 10,
            5, 6, 10,
            6, 10, 11,
            8, 9, 12,
            9, 12, 13
    };

    Cube(int x, int y, int z){ // Constructor of Cube takes its coordinates as input
        this->x = x;
        this->y = y;
        this->z = z;

        // Generate model matrix corresonding to this position
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z)); // Translates the cubes at the given positions
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Put the top of the texture on correct side
    }

    bool part(glm::vec3 pos){ // Checks if position "pos" is inside the cube, meaning it's less than 0.5 away from the center of the cube
        return abs(this->x - pos.x) <= 0.505 && abs(this->y - pos.y) <= 0.505 && abs(this->z - pos.z) <= 0.505;
    }
};
#endif