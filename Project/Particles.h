#ifndef PARTICLES_H
#define PARTICLES_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particles: Drawable{
public:
    static inline std::vector<glm::vec3> particle_positions;
    static inline float particle_speed;

    static inline std::vector<float> vertices = {
            // 2 components are 2D positions
            -0.002f, -0.01f,
            0.002f, 0.01f,
            0.002f, -0.01f,

            -0.002f, -0.01f,
            -0.002f, 0.01f,
            0.002f, 0.01f
    };

    Particles(std::string path_to_current_folder, glm::vec3 camera_pos, float speed_rainfall, int nb_drops, int area_drops):
        Drawable(Particles::vertices, 0, {}, {2}),
        shader(path_to_current_folder + "vertex_shader_particles.txt", path_to_current_folder + "fragment_shader_particles.txt")
    {
        this->area_drops = area_drops;
        this->particle_speed = speed_rainfall;
        create_particles(nb_drops, camera_pos);
    }

    void draw_particles(glm::mat4 view, glm::mat4 projection, glm::vec3 camera_pos){
        shader.use();
        shader.set_uniform("color", glm::vec3(0.38f, 0.85f, 0.90f));

        glm::vec3 particle_pos = particle_positions[0];
        // Calculate rotation to make sure the particle is always facing the user (in the y = cst plane)
        glm::vec3 origin_vector = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 vector_towards_camera = glm::normalize(glm::vec3(camera_pos[0], 0.0f, camera_pos[2]) - glm::vec3(particle_pos[0], 0.0f, particle_pos[2]));
        glm::mat4 rotation = glm::mat4(1.0f);
        if (origin_vector != vector_towards_camera) rotation = glm::rotate(rotation, acos(glm::dot(origin_vector, vector_towards_camera)), glm::normalize(glm::cross(origin_vector, vector_towards_camera)));
        // Theoretically, the rotation in "rotation" is only valid for the first drop
        // However turning all drops of this angle gives a realistics result so it is used for all drops to save computation time
        shader.set_uniform("rotation", rotation); // Apply rotation

        draw(particle_positions, view, projection, shader, -1, 6, GL_TRIANGLES, true, false); // -1 because we don't want a texture, instanced drawing
    }

    void update_positions(float delta_time, glm::vec3 camera_pos){
        for (int i = 0; i < particle_positions.size(); i++){
            particle_positions[i][1] -= particle_speed*delta_time; // Decrease y component to make the rain fall
            if (particle_positions[i][1] <= 0){
                // The area where drops are created follows the camera
                float x_pos = (rand()%(2*area_drops*10))/10.0f - area_drops + camera_pos[0]; // *10 in the modulo then /10 to have random numbers with random first decimal
                float y_pos = (rand()%(20*10))/10.0f + camera_pos[1]; // Once a rain drop is on the ground level, make it appear high again with a random height to take a random time to fall back
                float z_pos = (rand()%(2*area_drops*10))/10.0f - area_drops + camera_pos[2];
                particle_positions[i][0] = x_pos;
                particle_positions[i][1] = y_pos;
                particle_positions[i][2] = z_pos;
            }
        }
    }

private:
    Shader shader;
    int area_drops; // Rain appears in a area_drops x area_drops zone around the user

    void create_particles(int num_particles, glm::vec3 camera_pos){
        for (int i = 0; i < num_particles; i++){
            // Only create rain in a 10x10 area around the camera. The area where drops are created follows the camera
            float x_pos = (rand()%(2*area_drops*10))/10.0f - area_drops + camera_pos[0]; // *10 in the modulo then /10 to have random numbers with random first decimal
            float y_pos = (rand()%(20*10))/10.0f + camera_pos[1]; // Random height to take a random time to fall back to Earth
            float z_pos = (rand()%(2*area_drops*10))/10.0f - area_drops + camera_pos[2];
            particle_positions.push_back(glm::vec3(x_pos, y_pos, z_pos));
        }
    }
};
#endif


