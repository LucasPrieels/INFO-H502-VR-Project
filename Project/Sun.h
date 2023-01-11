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
    static inline std::vector<float> vertices = { // Large cube
            -2.0f, 2.0f, 2.0f,
            2.0f, 2.0f, 2.0f,
            -2.0f, 2.0f, 2.0f,
            -2.0f, -2.0f, 2.0f,
            2.0f, -2.0f, 2.0f,
            2.0f, 2.0f, 2.0f,
            -2.0f, 2.0f, 2.0f,
            -2.0f, 2.0f, -2.0f,
            -2.0f, -2.0f, -2.0f,
            2.0f, -2.0f, -2.0f,
            2.0f, 2.0f, -2.0f,
            -2.0f, 2.0f, -2.0f,
            -2.0f, 2.0f, -2.0f,
            2.0f, 2.0f, -2.0f
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
    glm::mat4 view_light, projection_light;

    Sun(std::string path_to_current_folder, glm::vec3 light_color, float distance_to_origin):
            Drawable(Sun::vertices, true, Sun::vertices_indices,{3}),
            shader(path_to_current_folder + "vertex_shader_light_source.txt", path_to_current_folder + "fragment_shader_light_source.txt")
    {
        // Init shader
        shader.use();
        this->light_color = light_color;
        this->distance_to_origin = distance_to_origin; // Set the distance the sun is from the origin of axis
    }

    void draw_sun(glm::mat4 view, glm::mat4 projection, float time_of_day, float day_duration, glm::vec3 camera_position){
        // Apply a rotation on the view to make the sun rotate depending on the time of day
        float angle_rot = glm::radians((time_of_day*1000/day_duration-0.05)*360); // -0.05 because the morning is 10% of the duration of the whole day and we want the sun to appear on the horizon at the middle of the morning
        light_pos = distance_to_origin * glm::vec3(0.0f, sin(angle_rot), -cos(angle_rot)) + camera_position;

        // Make the sun orange during sunrise and sunset
        float limit_angle_pos = M_PI/12; // When the angle of the sun wrt to the ground is smaller than this, it will become orange-ish (more orange as the angle is close to 0)
        float limit_angle_neg = -M_PI/20; // Same but when the sun is below ground (we don't want the ambient light to appear orange too long before the sun rises)
        if (angle_rot > limit_angle_neg && angle_rot < 0){ // Morning -> make the sunlight more orange, with the sun below the ground
            light_color = glm::mix(glm::vec3(1.0f, 0.5f, 0.020f), glm::vec3(1.0f), angle_rot/limit_angle_neg);
        }
        else if (angle_rot < limit_angle_pos && angle_rot >= 0){ // Morning -> make the sunlight more orange, with the sun above the ground
            light_color = glm::mix(glm::vec3(1.0f, 0.5, 0.020f), glm::vec3(1.0f), angle_rot/limit_angle_pos);
        }
        else if (angle_rot-M_PI > -limit_angle_pos && angle_rot-M_PI <= 0){ // Evening -> make the sunlight more orange, with the sun above the ground
            light_color = glm::mix(glm::vec3(1.0f, 0.5, 0.020f), glm::vec3(1.0f), abs(angle_rot-M_PI)/limit_angle_pos);
        }
        else if (angle_rot-M_PI < -limit_angle_neg && angle_rot-M_PI > 0){ // Evening -> make the sunlight more orange, with the sun below the ground
            light_color = glm::mix(glm::vec3(1.0f, 0.5, 0.020f), glm::vec3(1.0f), (angle_rot-M_PI)/abs(limit_angle_neg));
        }
        else{
            light_color = glm::vec3(1.0f); // Day or night light
        }

        shader.use();
        shader.set_uniform("light_color", light_color);
        draw({light_pos}, view, projection, shader, -1, 36, GL_TRIANGLES, false);
    }

private:
    Shader shader;
    float distance_to_origin;
};
#endif