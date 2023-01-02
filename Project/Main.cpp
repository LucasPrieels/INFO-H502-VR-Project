#include <iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string

// Texture
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Input_listener.h"
#include "Camera.h"
#include "Texture.h"

#define PATH "../../Project/" // Path to go from where the program is run to current folder

 int width = 800, height = 500; // Size of screen

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float cube_vertices[] = {
        -0.5f, 0.5f, 0.5f, 0.25f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.666f,
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

unsigned int cube_indices[] = {
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

float axis_lines[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f
};

GLFWwindow* init_window(){
    // Create OpenGL context
    if (!glfwInit()) throw std::runtime_error("Failed to initialise GLFW \n");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Use version 4.0 of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use Core version of OpenGL
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window and check it is working
    GLFWwindow* window = glfwCreateWindow(width, height, "VR Project", nullptr, nullptr); // Seems that the window doesn't take the specified size TODO
    if (window == NULL){
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }
    glfwMakeContextCurrent(window);

    return window;
}

unsigned int generate_VAO_cubes(){
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

unsigned int generate_VAO_axis(){
    unsigned int VAO_axis, VBO;
    glGenVertexArrays(1, &VAO_axis);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO_axis); // Bind VAO_lines to store next commands

    // Set cube_lines in VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_lines), axis_lines, GL_STATIC_DRAW);

    // Set position attributes in VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Deactivate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind the VBO and then VAO otherwise the VAO will store the unbinding of VBO
    glBindVertexArray(0); // Unbind VAO_lines: stop storing commands

    // De-allocate
    glDeleteBuffers(1, &VBO);

    return VAO_axis;
}

void draw_cubes(glm::vec3 cube_positions[], int NUM_CUBES_SIDE, unsigned int VAO_cubes, unsigned int texture, glm::mat4 view, glm::mat4 projection, Shader shader_texture){
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO_cubes);

    for (int i = 0; i < NUM_CUBES_SIDE*NUM_CUBES_SIDE; i++){
        // Model: move cubes at the given positions and rotate them
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        model = glm::translate(model, cube_positions[i]); // Translates the cubes at the given positions
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Put the top of the texture on correct side
        //model = glm::rotate(model, glm::radians(-10*(float)current_time), glm::vec3(0.0f, 1.0f, 0.0f));

        // Set uniforms in shader_texture
        shader_texture.set_uniform("model", model);
        shader_texture.set_uniform("view", view);
        shader_texture.set_uniform("projection", projection);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw one cube (3 vertices per triangle, 2 triangles per side, 6 sides per cube: 36 vertices per cube)
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void draw_axis(unsigned int VAO_axis, glm::mat4 view, glm::mat4 projection, Shader shader_color){
    shader_color.use();
    glBindVertexArray(VAO_axis);

    // Set uniforms in shader_texture
    shader_color.set_uniform("model", glm::mat4(1.0f));
    shader_color.set_uniform("view", view);
    shader_color.set_uniform("projection", projection);

    glDrawArrays(GL_LINES, 0, 6);
}

int main(int argc, char* argv[]){
    GLFWwindow* window = init_window();

    // Load OpenGL functions thanks to GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) throw std::runtime_error("Failed to initialize GLAD");
    glfwGetFramebufferSize(window, &width, &height); // Get real width and height of window (might be different from what we asked for because of Retine displays)
    glViewport(0, 0, width, height); // Set window size to let OpenGL transform from normalized coordinates

    // Creates input listener, camera, and compile shaders
    Input_listener::staticConstructor(window);
    Camera::staticConstructor();
    std::string path_string = PATH;
    Shader shader_texture(path_string + "vertex_shader_texture.txt", path_string + "fragment_shader_texture.txt");
    Shader shader_color(path_string + "vertex_shader_color.txt", path_string + "fragment_shader_color.txt");

    // Positions where to place cubes
    int NUM_CUBES_SIDE = 100; // We create a square of NUM_CUBES_SIDE x NUM_CUBES_SIDE
    glm::vec3 cube_positions[NUM_CUBES_SIDE*NUM_CUBES_SIDE];
    for (int i = 0; i < NUM_CUBES_SIDE; i++){
        for (int j = 0; j < NUM_CUBES_SIDE; j++){
            cube_positions[i*NUM_CUBES_SIDE+j] = glm::vec3((float)i, -3.0f, (float)j);
        }
    }

    unsigned int VAO_cubes = generate_VAO_cubes();
    unsigned int VAO_axis = generate_VAO_axis();

    Texture texture(path_string + "Textures/texture.jpg"); // 0 is index number of texture
    shader_texture.use();
    shader_texture.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0 so we write as uniform

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing to know which triangles are more in front

    // Render loop
    while (!glfwWindowShouldClose(window)){
        // Checks if a key is being pressed
        Input_listener::processInput(window, deltaTime);

        // Calculates and prints FPS
        double current_time = glfwGetTime(); // Time in s since beginning of code running
        deltaTime = current_time-lastFrame;
        double fps = 1/deltaTime;
        lastFrame = current_time;
        std::cout << "FPS: " << fps << std::endl;

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        shader_texture.use();

        // View: move world view on camera space
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(Camera::camera_pos, Camera::camera_pos+Camera::camera_front, Camera::camera_up);
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it

        // Projection: project 3D view on 2D
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

        draw_cubes(cube_positions, NUM_CUBES_SIDE, VAO_cubes, texture.texture_ID, view, projection, shader_texture);
        draw_axis(VAO_axis, view, projection, shader_color);

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen
    }
    // De-allocate
    glDeleteVertexArrays(1, &VAO_cubes);
    glDeleteVertexArrays(1, &VAO_axis);

    glfwTerminate(); // Clean GLFW resources
    return 0;
}
