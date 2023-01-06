#ifndef SUN_H
#define SUN_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Drawable.h"

class Sun: public Drawable{
public:
    static inline std::vector<float> vertices = {
            -0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f
    };

    static inline std::vector<unsigned int> vertices_indices = {
            0, 1, 3,
            1, 3, 4,
            2, 3, 7,
            3, 7, 8,
            3, 4, 8,
            4, 8, 9,
            4, 5, 9,
            5, 9, 10,
            5, 6, 10,
            6, 10, 11,
            8, 9, 12,
            9, 12, 13
    };

    glm::vec3 light_color, light_pos;

    Sun(std::string path_to_current_folder, glm::vec3 light_color, float distance_to_origin):
            Drawable(Sun::vertices, true, Sun::vertices_indices,{3}),
            shader(path_to_current_folder + "vertex_shader_light_source.txt", path_to_current_folder + "fragment_shader_light_source.txt")
    {

        // Init shader
        shader.use();
        this->light_color = light_color;
        this->distance_to_origin = distance_to_origin; // Set the distance the sun is from the origin of axis
    }

    void draw_sun(glm::mat4 view, glm::mat4 projection, float time_of_day, float day_duration){
        // Apply a rotation on the view to make the sun rotate depending on the time of day
        float angle_rot = glm::radians((time_of_day*1000/day_duration-0.05)*360); // -0.05 because the morning is 10% of the duration of the whole day and we want the sun to appear on the horizon at the middle of the morning
        light_pos = distance_to_origin * glm::vec3(0.0f, sin(angle_rot), -cos(angle_rot));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(2.0f));

        shader.use();
        shader.set_uniform("light_color", light_color);
        draw({model}, view, projection, shader, -1, 36, GL_TRIANGLES);
    }

private:
    Shader shader;
    float distance_to_origin;
};
#endif