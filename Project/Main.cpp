#include <iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <vector>

// Texture
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Input_listener.h"
#include "Camera.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Cube.h"
#include "Axis.h"
#include "Window.h"

#define PATH "../../Project/" // Path to go from where the program is run to current folder
// #define DAY_DURATION 20000 // Duration of a game day in ms
#define MOUSE_SENSITIVITY 0.05 // Sensitivity of yaw and pitch wrt mouse movements
#define RECOMPUTE_MODELS 10 // Recompute block models only once every RECOMPUTE_MODELS frames to gain time

static int width = 800, height = 500; // Size of screen

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
std::string path_string = PATH;


double fps(){
    // Calculates and prints FPS
        double current_time = glfwGetTime(); // Time in s since beginning of code running
        deltaTime = current_time-lastFrame;
        double fps = 1/deltaTime;
        lastFrame = current_time;
        std::cout << "FPS: " << fps << std::endl;
        return current_time;
}


int main(int argc, char* argv[]){

    GLFWwindow* window = Window::init_window(width, height);
    Window::loadWindow(window, width, height);

    // Creates input listener, camera, and compile shaders
    Input_listener::staticConstructor(window, MOUSE_SENSITIVITY);
    Camera::staticConstructor();


    Shader shader_texture(path_string + "vertex_shader_texture.txt", path_string + "fragment_shader_texture.txt");
    Shader shader_color(path_string + "vertex_shader_color.txt", path_string + "fragment_shader_color.txt");

    Texture texture(path_string + "Textures/texture.jpg"); // 0 is index number of texture
    shader_texture.use();
    shader_texture.set_uniform("texture_uniform", 0); // Bound texture will be put at index 0 so we write as uniform
    
 // Positions where to place cubes: create a square of NUM_CUBES_SIDE x NUM_CUBES_SIDE
    glm::vec3 cube_positions[NUM_CUBES_SIDE*NUM_CUBES_SIDE];
    Cube cube;
    cube.cubePositions(cube_positions);
    unsigned int VAO_cubes = cube.generateVAOCubes();
    
    Axis axis;
    unsigned int VAO_axis = axis.generate_VAO_axis();


    std::vector<std::string> faces_night = { // Filenames of skybox
        path_string + "Textures/skybox/night_right.png",
        path_string + "Textures/skybox/night_left.png",
        path_string + "Textures/skybox/night_top.png",
        path_string + "Textures/skybox/night_bottom.png",
        path_string + "Textures/skybox/night_front.png",
        path_string + "Textures/skybox/night_back.png"
    };

    Cubemap cubemap(faces_night);

    Shader shader_skybox(path_string + "vertex_shader_skybox.txt", path_string + "fragment_shader_skybox.txt");
    shader_skybox.use();
    shader_skybox.set_uniform("color_day", glm::vec3(0.05f, 0.4f, 0.9f)); // Set color to use for the sky during the day
    shader_skybox.set_uniform("skybox_night", 0); // Set texture to use for the sky during the night
    unsigned int VAO_skybox = cubemap.generate_VAO_skybox();

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing to know which triangles are more in front

    // Models: compute only once every RECOMPUTE_MODELS frames to gain time
    glm::mat4 models [NUM_CUBES_SIDE*NUM_CUBES_SIDE];
    cube.recompute_models_blocks(models, cube_positions);

    // Render loop
    int frame_nb = 0;
    while (!glfwWindowShouldClose(window)){
        frame_nb++;

        // Checks if a key is being pressed
        Input_listener::processInput(window, deltaTime);

        double current_time = fps(); // Calculates and prints FPS

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

        // Model: recompute block models only once every RECOMPUTE_MODELS frames
        if (frame_nb % RECOMPUTE_MODELS == 0){
            cube.recompute_models_blocks(models, cube_positions);
        }

        // View: move world view on camera space
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(Camera::camera_pos, Camera::camera_pos+Camera::camera_front, Camera::movement_up);
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it

        // Projection: project 3D view on 2D
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

        cubemap.draw_skybox(VAO_skybox, cubemap.cubemap_ID, view, projection, current_time, shader_skybox); // current_time used to blend day color and night texture during morning and evening
        cube.draw_cubes(cube_positions, VAO_cubes, texture.texture_ID, models, view, projection, shader_texture);
        axis.draw_axis(VAO_axis, view, projection, shader_color);

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen
    }
    // De-allocate
    glDeleteVertexArrays(1, &VAO_cubes);
    glDeleteVertexArrays(1, &VAO_axis);

    glfwTerminate(); // Clean GLFW resources
    return 0;
}


