#ifndef TARGET_H
#define TARGET_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Target: public Drawable{
    public:
    static inline std::vector<float> vertices = { // Target in the middle of the camera (already in the camera space, view and projection are identity)
            0.0f, -0.05f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.05f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.05f*(float)Window::height/(float)Window::width, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.05f*(float)Window::height/(float)Window::width, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

    Target(std::string path_to_current_folder):
    Drawable(Target::vertices, false, {},{3, 3}),
    shader(path_to_current_folder + "vertex_shader_color.txt", path_to_current_folder + "fragment_shader_color.txt")
    { // Create a 3-axis system at (0,0,0) in the map
        // Init shader
        shader.use();
        

    }

    void draw_axis(){
        // Send matrices for the target to stay in the middle of the screen
        glDepthMask(GL_FALSE); // Needed otherwise when we click the detected depth is 0 (because the target is in front of all other objects)
        draw({glm::vec3(0.0f)}, glm::mat4{1.0f}, glm::mat4{1.0f}, shader, -1, 4, GL_LINES, false, false); // -1 because we don't want a texture
        glDepthMask(GL_TRUE);
    }

private:
    Shader shader;
};
#endif