#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Axis{

    float axis_lines[36] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f
    };
    
    public:

    unsigned int generate_VAO_axis(){
        unsigned int VAO_axis, VBO;
        glGenVertexArrays(1, &VAO_axis);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO_axis); // Bind VAO to store next commands

        // Set axis_lines in VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(axis_lines), axis_lines, GL_STATIC_DRAW);

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

    void draw_axis(unsigned int VAO_axis, glm::mat4 view, glm::mat4 projection, Shader shader_color){
        shader_color.use();
        glBindVertexArray(VAO_axis);

        // Set uniforms in shader_texture
        shader_color.set_uniform("model", glm::mat4(1.0f));
        shader_color.set_uniform("view", view);
        shader_color.set_uniform("projection", projection);

        glDrawArrays(GL_LINES, 0, 6);
    }


};