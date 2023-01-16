#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Drawable.h"
#include  "Model.h"


struct Text{
    unsigned int Texture_ID;
    std::string type;
    std::string path;
};

class Mesh: public Drawable{
    public:

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<Text> textures;


    Mesh(std::string path_to_current_folder, std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<Text> textures):

        Drawable(vertices, true, indices, {3, 3, 2}),
        shader(path_to_current_folder + "vertex_shader_model.txt", path_to_current_folder + "fragment_shader_model.txt")
        {
        this->indices = indices;
        this->textures = textures;
        this->vertices = vertices;
  
        }

    void drawMesh(Shader &shader){
        unsigned int diffuse_number = 1;
        unsigned int specular_number = 1;
        for (int i=0; i < textures.size(); i++){
            glActiveTexture(GL_TEXTURE0+i); //activate proper texture
            std::string number;
            std::string name = textures[i].type;
            if(name== "texture_diffuse") number = std::to_string(diffuse_number++);
            else if (name== "texture_specular") number = std::to_string(specular_number++);

            shader.set_uniform((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].Texture_ID);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    private:
    Shader shader;

};
#endif