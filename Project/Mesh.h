#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
//#include "Drawable.h"
#include  "Model.h"
#define MAX_BONE_WEIGHTS 4

struct Text{
    unsigned int Texture_ID;
    std::string type;
    std::string path;
};

// struct Veretex{
//     glm::vec3 position;
//     glm::vec3 normals;
//     glm::vec2 texCoords;

//     int m_boneIds[MAX_BONE_WEIGHTS];
//     float boneWeight[MAX_BONE_WEIGHTS];
// }

class Mesh{
public:
    std::vector<float> vertices;
    std::vector<unsigned int> vertices_indices;
    std::vector<Text> textures;

    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<Text> textures){
        this->vertices_indices = indices;
        this->textures = textures;
        this->vertices = vertices;
        VAO = generate_VAO();
    }

     unsigned int generate_VAO() {
        // Create a VAO_cubes that draw cubes
        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // if (use_EBO) 

        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO); // Bind VAO to store next commands

        // Set vertices in VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        float vertices_array[vertices.size()];
        std::copy(vertices.begin(), vertices.end(), vertices_array); // Copies content of vertices (a vector) to vertices_array (an array, needed for function glBufferData)
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_array), vertices_array, GL_STATIC_DRAW);

        // If use_EBO is true, set vertices_indices in EBO
        // if (use_EBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            int vertices_indices_array[vertices_indices.size()];
            std::copy(vertices_indices.begin(), vertices_indices.end(), vertices_indices_array); // Copies content of vertices_indices (a vector) to vertices_indices_array (an array, needed for function glBufferData)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_indices_array), vertices_indices_array, GL_STATIC_DRAW);
        // }

        // Set position attributes in VAO, to tell shaders how to interpret input data
        unsigned int stride = 0;
        for (unsigned int position_attribute: position_attributes) stride += position_attribute;

        unsigned int current_index = 0;
        for (int i = 0; i < position_attributes.size(); i++) {

            if (i == 3){
                glVertexAttribPointer(i, position_attributes[i], GL_INT, GL_FALSE, stride * sizeof(float), (void *) (current_index * sizeof(int)));
            }
            else{
                glVertexAttribPointer(i, position_attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) (current_index * sizeof(float)));
            }
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


    void drawMesh(Shader &shader){
        unsigned int diffuse_number = 1;
        unsigned int specular_number = 1;

        for (int i=0; i < textures.size(); i++){
            glActiveTexture(GL_TEXTURE0+i); //activate proper texture
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse") number = std::to_string(diffuse_number++);
            else if (name == "texture_specular") number = std::to_string(specular_number++);

            shader.set_uniform((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].Texture_ID);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO);
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 6.5f, -4.0f));
        model = glm::scale(model, glm::vec3(0.08f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        shader.set_uniform("model", model);
        glDrawElements(GL_TRIANGLES, vertices_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    private:
    std::string path_to_current_folder;
    unsigned int VAO; // VAO used to draw the object
    std::vector<unsigned int> position_attributes = {3, 3, 2}/*, 4, 4}*/; // Sequence of attribute positions: for example if = {3, 2} we set glVertexAttribPointer with 3 floats followed by 2 floats
};
#endif