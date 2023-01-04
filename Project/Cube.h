#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"
#define NUM_CUBES_SIDE 100


class Cube{
    public:
    unsigned int VAO_cube;
    

    float cube_vertices[70] = {
        -0.5f, 0.5f, 0.5f, 0.25f, 1.0f, //top left front
        0.5f, 0.5f, 0.5f, 0.5f, 1.0f,   //top right front
        -0.5f, 0.5f, 0.5f, 0.0f, 0.666f,//
        -0.5f, -0.5f, 0.5f, 0.25f, 0.666f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.666f,
        0.5f, 0.5f, 0.5f, 0.75f, 0.666f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.666f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.334f,
        -0.5f, -0.5f, -0.5f, 0.25f, 0.334f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.334f,
        0.5f, 0.5f, -0.5f, 0.75f, 0.334f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.334f,
        -0.5f, 0.5f, -0.5f, 0.25f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.0f
};

unsigned int cube_indices[36] = {
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

    unsigned int generateVAOCubes(){
        // Create a VAO_cubes that draw cubes
        unsigned int VBO, VAO_cubes, EBO;
        glGenVertexArrays(1, &VAO_cubes);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO_cubes); // Bind VAO_cubes to store next commands

        // Set cube_vertices in VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        // Set cube_indices in EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

        // Set position attributes in VAO
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Deactivate the buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind the VBO and then VAO_cubes otherwise the VAO_cubes will store the unbinding of VBO
        glBindVertexArray(0); // Unbind VAO_cubes: stop storing commands

        // De-allocate
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        
        return VAO_cubes;
    }
    
void draw_cubes(std::vector<glm::vec3> cube_positions, unsigned int VAO_cubes, unsigned int texture, glm::mat4 models[], glm::mat4 view, glm::mat4 projection, Shader shader_texture){
    // Instead of re-computing glm::mat4 model at each frame, we do it less often and directly give the matrices to this function
    // Bind texture
    shader_texture.use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO_cubes);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set uniforms in shader_texture
    shader_texture.set_uniform("view", view);
    shader_texture.set_uniform("projection", projection);

    for (int i = 0; i < cube_positions.size(); i++){
        // Set uniforms in shader_texture
        shader_texture.set_uniform("model", models[i]);

        // Draw one cube (3 vertices per triangle, 2 triangles per side, 6 sides per cube: 36 vertices per cube)
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

std::vector<glm::vec3> cubePositions(std::vector<glm::vec3> cube_positions){
    float altitude;
    
    for (int i = -NUM_CUBES_SIDE/2; i < NUM_CUBES_SIDE/2; i++){
        for (int j = -NUM_CUBES_SIDE/2; j < NUM_CUBES_SIDE/2; j++){

            altitude =round(3*cos(((float)i+(float)j)/NUM_CUBES_SIDE*2*3.1481)-5);

            for (float k =-10.0; k<= altitude; k++){
                cube_positions.push_back(glm::vec3((float)i, k, (float)j)) ;
            }   
        }
    }
    return cube_positions;
}


void recompute_models_blocks(glm::mat4* models, std::vector<glm::vec3> cube_positions){
    // Move cubes at the given positions and rotate them
    for (int i = 0; i < cube_positions.size(); i++){
        models[i] = glm::mat4(1.0f);
        models[i] = glm::translate(models[i], cube_positions[i]); // Translates the cubes at the given positions
        models[i] = glm::rotate(models[i], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Put the top of the texture on correct side
    }
}

};