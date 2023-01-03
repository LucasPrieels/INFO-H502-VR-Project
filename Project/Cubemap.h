#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <fstream>
#include "Input_listener.h"

class Cubemap{
public:
    unsigned int cubemap_ID;

    Cubemap(std::vector<std::string> cubemap_faces){
        glGenTextures(1, &cubemap_ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_ID);

        int texture_width, texture_height, channels_nb;
        for (unsigned int i = 0; i < cubemap_faces.size(); i++){
            unsigned char *data = stbi_load(cubemap_faces[i].c_str(), &texture_width, &texture_height, &channels_nb, 0); // Load all skybox images
            if (data){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Apply each image to a specific plane
            }
            else{
                std::cout << "Error while loading cubemap texture file " << cubemap_faces[i] << std::endl;
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
};

#endif