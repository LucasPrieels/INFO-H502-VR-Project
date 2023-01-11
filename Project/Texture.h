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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture{
public:
    static inline std::vector<Texture> textures;
    unsigned int texture_ID;
    float shininess; // Represents the amount of specular light reflected by this material texture
    bool opaque; // Whether this texture is completely opaque or not
    bool mirror; // Whether this texture is a mirror or not
    int framebuffer; // If the texture is a mirror, this contains the index of the framebuffer related to this texture
    glm::vec3 position; // If the texture is a mirror, this contains the position of the view point from which this texture is taken
    glm::vec3 direction; // Same with the direction of the view point

    Texture(std::string filename, float shininess, bool opaque, glm::vec3 position, glm::vec3 direction, int resol_mirror){ // Position and direction are only used for mirror textures
        // Load and create texture
        glGenTextures(1, &texture_ID);
        glActiveTexture(GL_TEXTURE0); // Texture is bound to texture unit 0
        glBindTexture(GL_TEXTURE_2D, texture_ID);

        if (filename.substr(filename.size()-6, 6) == "mirror"){ // Mirror textures are a particular case where data = NULL
            // Create new framebuffer for each mirror texture
            glGenFramebuffers(1, (unsigned int*)&framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resol_mirror, resol_mirror, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // resol_mirror is the dimensions of the taken image that will be put on the mirror cube sides
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_ID, 0);

            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resol_mirror, resol_mirror);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "Error: framebuffer is not complete" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            this->shininess = shininess;
            this->opaque = opaque;
            mirror = true;
            this->position = position;
            this->direction = direction;

            // Each time we create a new texture we add it to the list of textures
            Texture::textures.push_back(*this);
            return;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width_texture, height_texture, channels_nb;
        unsigned char *data = stbi_load(filename.c_str(), &width_texture, &height_texture, &channels_nb, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_texture, height_texture, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        this->shininess = shininess;
        this->opaque = opaque;
        mirror = false;

        // Each time we create a new texture we add it to the list of textures
        Texture::textures.push_back(*this);
    }
};

#endif