#ifndef TARGET_H
#define TARGET_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Target{
    public:
    float target_lines[24] = { // Target in the middle of the camera (already in the camera space, view and projection are identity)
            0.0f, -0.05f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.05f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.05f*(float)Window::height/Window::width, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.05f*(float)Window::height/Window::width, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

    unsigned int generate_VAO_lines(float lines[], int size_lines){
        unsigned int VAO_axis, VBO;
        glGenVertexArrays(1, &VAO_axis);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO_axis); // Bind VAO to store next commands

        // Set lines in VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size_lines, lines, GL_STATIC_DRAW);

        // Set position attributes in VAO
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Deactivate the buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind the VBO and then VAO otherwise the VAO will store the unbinding of VBO
        glBindVertexArray(0); // Unbind VAO: stop storing commands

        // De-allocate
        glDeleteBuffers(1, &VBO);

        return VAO_axis;
    }

    void draw_target(unsigned int VAO_target, Shader shader_color){ // Draw target in the middle of the screen
        glDepthMask(GL_FALSE);
        shader_color.use();
        glBindVertexArray(VAO_target);

        // Set uniforms in shader_texture
        shader_color.set_uniform("model", glm::mat4(1.0f));
        shader_color.set_uniform("view", glm::mat4(1.0f));
        shader_color.set_uniform("projection", glm::mat4(1.0f));

        glDrawArrays(GL_LINES, 0, 4);
        glDepthMask(GL_TRUE);
    }


};

#endif