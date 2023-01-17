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
#include "Shader.h"

class Cubemap: public Drawable{
public:
    unsigned int cubemap_ID;

    static inline std::vector<float> vertices = { // Cube but defined face by face (on the contrary of Cube::vertices)
        -1.0f,  1.0f, -1.0f, // Face z=-1 has normal in direction
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

    static inline std::vector<std::string> faces_night = { // Filenames of skybox
            "Textures/skybox/night_right.png",
            "Textures/skybox/night_left.png",
            "Textures/skybox/night_top.png",
            "Textures/skybox/night_bottom.png",
            "Textures/skybox/night_front.png",
            "Textures/skybox/night_back.png"
    };

    Cubemap(std::string path_to_current_folder):
        Drawable(Cubemap::vertices, false, {},{3}),
        shader(path_to_current_folder + "vertex_shader_skybox.txt", path_to_current_folder + "fragment_shader_skybox.txt")
    {
        // Init shader
        shader.use();
        shader.set_uniform("skybox_night", 0); // Set texture to use for the sky during the night

        glGenTextures(1, &cubemap_ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_ID);

        int texture_width, texture_height, channels_nb;
        for (unsigned int i = 0; i < faces_night.size(); i++){
            unsigned char *data = stbi_load((path_to_current_folder + faces_night[i]).c_str(), &texture_width, &texture_height, &channels_nb, 0); // Load all skybox images
            if (data){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Apply each image to a specific plane
            }
            else{
                std::cout << "Error while loading cubemap texture file " << path_to_current_folder + faces_night[i] << std::endl;
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void draw_skybox(glm::mat4 view, glm::mat4 projection, float current_time, int day_duration, bool sunny){
        shader.use();
        if (sunny) shader.set_uniform("color_day", glm::vec3(0.05f, 0.4f, 0.9f)); // Set color to use for the sky during the day
        else shader.set_uniform("color_day", glm::vec3(0.35, 0.35, 0.35f)); // Grey clouds

        float time_of_day = (int)round(1000*current_time)%day_duration; // In ms, 0 is start of morning
        // Depending on the time of day, the blend factor blending day and night skies will be different
        float blend_factor;
        // A day is made of 40% day, 10% evening, 40% night and 10% morning
        if (time_of_day < day_duration*0.1f){ // Morning
            blend_factor = time_of_day/(day_duration*0.1f);
        }
        else if (time_of_day < day_duration*0.5f){ // Day
            blend_factor =  1.0f;
        }
        else if (time_of_day < day_duration*0.6f){ // Evening
            blend_factor =(day_duration*0.6f-time_of_day)/(day_duration*0.1f);
        }
        else {// Night
            blend_factor = 0.0f;
        }

        // Calculate angle of rotation of the sky depending on the time of day
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, glm::radians(time_of_day/day_duration*360.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate sky to make 360 degrees in a day

        // Apply the rotation on the view to make the sky rotate
        glm::mat4 view_rotated = glm::mat4(glm::mat3(view))*rotation;

        shader.use();
        shader.set_uniform("blend_factor", blend_factor); // Set correct blending

        glDepthFunc(GL_LEQUAL); // In the Skybox shader we set z component = w component for the depth to be 1 (maximal), so we want to keep samples when they are equal to the max
        draw({glm::vec3(0.0f)}, view_rotated, projection, shader, cubemap_ID, 36, GL_TRIANGLES, false, false); // Print 6 vertices for each of the 6 cube faces
        glDepthFunc(GL_LESS);
    }
private:
    Shader shader;
};

#endif