#ifndef MIRROR_H
#define MIRROR_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Sun.h"

class Mirror: Drawable{
public:
    glm::vec3 position; // Coordinates of the mirror
    Texture texture;
    Shader shader;

    static inline std::vector<Mirror> mirrors;

    static inline std::vector<float> vertices_x_plus = { // Vertices for a mirror facing the X+ direction
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            -0.49f, -0.49f, 0.49f,     1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
            -0.49f, 0.49f, 0.49f,      1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
            -0.49f, -0.49f, -0.49f,    0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
            -0.49f, 0.49f, -0.49f,     0.0f, 1.0f,     1.0f, 0.0f, 0.0f
    };

    static inline std::vector<float> vertices_x_minus = { // Vertices for a mirror facing the X- direction
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            0.49f, -0.49f, -0.49f,     1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
            0.49f, 0.49f, -0.49f,      1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            0.49f, -0.49f, 0.49f,      0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
            0.49f, 0.49f, 0.49f,       0.0f, 1.0f,     -1.0f, 0.0f, 0.0f
    };

    static inline std::vector<float> vertices_y_minus = { // Vertices for a mirror facing the Y- direction
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            -0.49f, 0.49f, -0.49f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            -0.49f, 0.49f, 0.49f,      1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
            0.49f, 0.49f, -0.49f,      0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            0.49f, 0.49f, 0.49f,       0.0f, 1.0f,     0.0f, -1.0f, 0.0f
    };
    static inline std::vector<float> vertices_y_plus = { // Vertices for a mirror facing the Y+ direction
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            -0.49f, -0.49f, 0.49f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            -0.49f, -0.49f, -0.49f,    0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
            0.49f, -0.49f, 0.49f,      1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            0.49f, -0.49f, -0.49f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f
    };
    static inline std::vector<float> vertices_z_minus = { // Vertices for a mirror facing the Z- direction
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            0.49f, -0.49f, 0.49f,      1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
            0.49f, 0.49f, 0.49f,       1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            -0.49f, -0.49f, 0.49f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
            -0.49f, 0.49f, 0.49f,      0.0f, 1.0f,     0.0f, 0.0f, -1.0f
    };
    static inline std::vector<float> vertices_z_plus = { // Vertices for a mirror facing the Z+ direction
            // First 3 are 3D positions, next 2 are texture positions, final 3 are normal vector components
            -0.49f, -0.49f, -0.49f,    1.0f, 0.0f,     0.0f, 0.0f, 1.0f, // Invert x-component of texture to create the symmetry effect
            -0.49f, 0.49f, -0.49f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            0.49f, -0.49f, -0.49f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
            0.49f, 0.49f, -0.49f,      0.0f, 1.0f,     0.0f, 0.0f, 1.0f
    };

    static inline std::vector<unsigned int> vertices_indices = {
            2, 1, 0,
            1, 2, 3
    };

    Mirror(std::string path_to_current_folder, glm::vec3 position, glm::vec3 orientation, std::vector<float> vertices): // Vertices is one of the static vertices possibilities shown above
        Drawable(vertices, true, Mirror::vertices_indices, {3, 2, 3}),
        texture("mirror", 5.0f, true, position, orientation), // Mirror blocks have a shininess of 60
        shader(path_to_current_folder + "vertex_shader_texture.txt", path_to_current_folder + "fragment_shader_texture.txt")
    {
        this->position = position;
        Mirror::mirrors.push_back(*this);
    }

    void draw_mirror(glm::mat4 view, glm::mat4 projection, Sun sun, glm::vec3 camera_pos){
        shader.use();
        shader.set_uniform("light_color", sun.light_color);
        shader.set_uniform("light_pos", sun.light_pos);
        shader.set_uniform("viewing_pos", camera_pos);
        shader.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0, so we write as uniform
        shader.set_uniform("shininess", texture.shininess);
        draw({position}, view, projection, shader, texture.texture_ID, 6, GL_TRIANGLES, true);
    }

    static void draw_mirrors(glm::mat4 view, glm::mat4 projection, Sun sun, glm::vec3 camera_pos){
        glEnable(GL_CULL_FACE); // Improves computation power and allows to have leaves blocks without flickering
        for (Mirror mirror: Mirror::mirrors){
            mirror.draw_mirror(view, projection, sun, camera_pos);
        }
        glDisable(GL_CULL_FACE);
    }
};
#endif