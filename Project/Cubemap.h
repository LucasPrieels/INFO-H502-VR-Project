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

#define DAY_DURATION 20000 // Duration of a game day in ms

class Cubemap{
public:
    unsigned int cubemap_ID;


    float skyboxVertices[108] = { // Cube but defined face by face (on the contrary of cube_vertices)
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

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


    unsigned int generate_VAO_skybox(){
    unsigned int VAO_skybox, VBO;
    glGenVertexArrays(1, &VAO_skybox);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO_skybox); // Bind VAO to store next commands

    // Set cube_lines in VBO (we don't use the cube texture, only the vertices)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // Set position attributes in VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // There are also 2 texture values but we don't use them
    glEnableVertexAttribArray(0);

    // Deactivate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind the VBO and then VAO otherwise the VAO will store the unbinding of VBO
    glBindVertexArray(0); // Unbind VAO_lines: stop storing commands

    // De-allocate
    glDeleteBuffers(1, &VBO);

    return VAO_skybox;
}

void draw_skybox(unsigned int VAO_skybox, unsigned int texture_cubemap_night, glm::mat4 view, glm::mat4 projection, float current_time, Shader shader_skybox){
    glDepthMask(GL_FALSE);
    shader_skybox.use();
    glBindVertexArray(VAO_skybox);
    // Set uniforms in shader_skybox
    float time_of_day = (int)round(1000*current_time)%DAY_DURATION; // In ms, 0 is start of morning
    // A day is made of 40% day, 10% evening, 40% night and 10% morning
    if (time_of_day < DAY_DURATION*0.1f){ // Morning
        shader_skybox.set_uniform("blend_factor", time_of_day/(DAY_DURATION*0.1f));
    }
    else if (time_of_day < DAY_DURATION*0.5f){ // Day
        shader_skybox.set_uniform("blend_factor", 1.0f);
    }
    else if (time_of_day < DAY_DURATION*0.6f){ // Evening
        shader_skybox.set_uniform("blend_factor", (DAY_DURATION*0.6f-time_of_day)/(DAY_DURATION*0.1f));
    }
    else {// Night
        shader_skybox.set_uniform("blend_factor", 0.0f);
    }
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(time_of_day/DAY_DURATION*360), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate sky to make 360 degrees in a day
    shader_skybox.set_uniform("view", glm::mat4(glm::mat3(view))*rotation);
    shader_skybox.set_uniform("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap_night);
    glDrawArrays(GL_TRIANGLES, 0, 36); // Print 6 vertices for each of the 6 cube faces
    glDepthMask(GL_TRUE);
}


};

#endif