#ifndef AXIS_H
#define AXIS_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Drawable.h"

class Axis: public Drawable{
public:
    static inline std::vector<float> vertices = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f
    };

    Axis(std::string path_to_current_folder):
            Drawable(Axis::vertices, false, {},{3, 3}),
            shader(path_to_current_folder + "vertex_shader_color.txt", path_to_current_folder + "fragment_shader_color.txt")
    { // Create a 3-axis system at (0,0,0) in the map

        // Init shader
        shader.use();
    }

    void draw_axis(glm::mat4 view, glm::mat4 projection){
        // NPC is glm::mat4(1.0f) because we want to place the axis at the origin
        draw({glm::vec3(0.0f)}, view, projection, shader, -1, 6, GL_LINES, false, false); // -1 because we don't want a texture
    }

private:
    Shader shader;
};
#endif