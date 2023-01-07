#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Drawable{
public:
    Drawable(std::vector<float> vertices, bool use_EBO, std::vector<unsigned int> vertices_indices, std::vector<unsigned int> position_attributes){ // Constructor
        this->vertices = vertices;
        this->use_EBO = use_EBO;
        this->vertices_indices = vertices_indices; // Empty if use_EBO is false
        this->position_attributes = position_attributes; // If we want to store 3 floats as position data followed by 2 floats for texture for example, position_attributes = {3, 2}

        VAO = generate_VAO();
    }

    void draw(std::vector<glm::vec3> translations, glm::mat4 view, glm::mat4 projection, Shader shader, int texture, int num_vertices, int type_primitive, bool instanced) {
        // Draw the object using its VAO
        // Translations is a vector because we can draw many objects at different positions at once
        // Num_vertices is the number of vertices to draw per object (36 for a cube for example)
        // Type_primitive can for example be GL_TRIANGLES or GL_LINES
        // Texture is -1 if don't want to use a texture for this drawable object
        // Instanced states whether we want to use drawElementsInstanced/drawArraysInstanced (with the translation as input) or not (with model as uniform), in which case we always have translations.size()==1

        // Bind shader
        shader.use();

        // Bind texture if needed (if texture == -1 it means we don't want to use one)
        if (shader.vertex_shader_code == "vertex_shader_skybox.txt") glBindTexture(GL_TEXTURE_CUBE_MAP, texture); // To draw a skybox we use GL_TEXTURE_CUBE_MAP instead of GL_TEXTURE_2D
        if (texture >= 0) glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        // Set uniforms in shader
        shader.set_uniform("view", view);
        shader.set_uniform("projection", projection);

        if (translations.size() == 0) return; // Nothing to print
        if (instanced) {
            unsigned int instanceVBO;
            glGenBuffers(1, &instanceVBO);
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(glm::vec3), &translations[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void *) 0); // last 2 3 are because translations is a vec3
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

            // Set model uniforms in shader
            if (use_EBO) glDrawElementsInstanced(type_primitive, num_vertices, GL_UNSIGNED_INT, 0, translations.size());
            else glDrawArraysInstanced(type_primitive, 0, num_vertices, translations.size());
            glBindVertexArray(0);
        }
        else{
            // Set model uniforms in shader
            glm::mat4 model(1.0f);
            model = glm::translate(model, translations[0]); // For instanced==false we always have translations.size()==1
            shader.set_uniform("model", model);
            if (use_EBO) glDrawElements(type_primitive, num_vertices, GL_UNSIGNED_INT, 0);
            else glDrawArrays(type_primitive, 0, num_vertices);
        }
    }

private:
    unsigned int VAO; // VAO used to draw the object
    std::vector<float> vertices; // List of vertices
    bool use_EBO; // True if we use an EBO to select vertices in order
    std::vector<unsigned int> vertices_indices; // List of vertices indices (empty if use_EBO is false)
    std::vector<unsigned int> position_attributes; // Sequence of attribute positions: for example if = {3, 2} we set glVertexAttribPointer with 3 floats followed by 2 floats

    unsigned int generate_VAO() {
        // Create a VAO_cubes that draw cubes
        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (use_EBO) glGenBuffers(1, &EBO);

        glBindVertexArray(VAO); // Bind VAO to store next commands

        // Set vertices in VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        float vertices_array[vertices.size()];
        std::copy(vertices.begin(), vertices.end(), vertices_array); // Copies content of vertices (a vector) to vertices_array (an array, needed for function glBufferData)
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_array), vertices_array, GL_STATIC_DRAW);

        // If use_EBO is true, set vertices_indices in EBO
        if (use_EBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            int vertices_indices_array[vertices_indices.size()];
            std::copy(vertices_indices.begin(), vertices_indices.end(), vertices_indices_array); // Copies content of vertices_indices (a vector) to vertices_indices_array (an array, needed for function glBufferData)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_indices_array), vertices_indices_array, GL_STATIC_DRAW);
        }

        // Set position attributes in VAO, to tell shaders how to interpret input data
        unsigned int stride = 0;
        for (unsigned int position_attribute: position_attributes) stride += position_attribute;

        unsigned int current_index = 0;
        for (int i = 0; i < position_attributes.size(); i++) {
            glVertexAttribPointer(i, position_attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(float),
                                  (void *) (current_index * sizeof(float)));
            glEnableVertexAttribArray(i);
            current_index += position_attributes[i];
        }

        // Deactivate the buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind the VBO and then VAO otherwise the VAO will store the unbinding of VBO
        glBindVertexArray(0); // Unbind VAO: stop storing commands

        // De-allocate
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        return VAO;
    }
};
#endif