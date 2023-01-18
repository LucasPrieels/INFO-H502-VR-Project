#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for to_std::string
#include "Shader.h"
#include "NPC.h"

struct Tex {
    unsigned int ID;
    std::string type;
    std::string path;
};

class Mesh: Drawable {
public:
    std::vector<Tex> textures;
    std::vector<unsigned int> indices;

    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<Tex> textures):
        Drawable(vertices, true, indices, {3, 3, 2})
    {
        this->textures = textures;
        this->indices = indices;
    }

    void draw_mesh(glm::vec3 translation, glm::mat4 view, glm::mat4 projection, Shader &shader){
        int diffuse_nb = 1;
        int specular_nb = 1;
        for(int i = 0; i < textures.size(); i++){
            // For each of the possible textures, bind to the i-th texture unit
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number_texture;
            std::string name_texture = textures[i].type;
            if (name_texture == "texture_diffuse"){
                number_texture = std::to_string(diffuse_nb);
                diffuse_nb++;
            }
            else if (name_texture == "texture_specular"){
                number_texture = std::to_string(specular_nb);
                specular_nb++;
            }

            shader.set_uniform(name_texture + number_texture, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].ID);
        }

        draw({translation}, view, projection, shader, -1, indices.size(), GL_TRIANGLES, false, false);
        // Texture are already set by the previous loop, so we can put -1 in draw
        glActiveTexture(GL_TEXTURE0);
    }
};
#endif