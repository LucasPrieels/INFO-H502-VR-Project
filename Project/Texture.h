#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <fstream>
#include "Input_listener.h"

class Texture{
public:
    unsigned int texture_ID;

    Texture(std::string filename){
        // Load and create texture
        glGenTextures(1, &texture_ID);
        glBindTexture(GL_TEXTURE_2D, texture_ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width_texture, height_texture, nrChannels;
        unsigned char *data = stbi_load(filename.c_str(), &width_texture, &height_texture, &nrChannels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_texture, height_texture, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
};

#endif