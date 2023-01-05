#include <iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <vector>
#include "Map.h"

#include "Shader.h"
#include "Input_listener.h"
#include "Camera.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Cube.h"
#include "Axis.h"
#include "Window.h"
#include "Target.h"

#define PATH "../../Project/" // Path to go from where the program is run to current folder
// #define DAY_DURATION 20000 // Duration of a game day in ms
#define MOUSE_SENSITIVITY 0.05 // Sensitivity of yaw and pitch wrt mouse movements
#define MAX_DISTANCE_REMOVE 15 // We only remove clicked blocks up to this distance
#define NUM_CUBES_SIDE 80

 int width = 1600, height = 1000; // Size of screen

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

    Axis axis;
    unsigned int VAO_axis = axis.generate_VAO_axis();
    Target target;
    unsigned int VAO_target = target.generate_VAO_lines(target.target_lines, sizeof(target.target_lines)); // Same shaders

    // Creates input listener, camera, and compile shaders
    Map map(NUM_CUBES_SIDE, path_string);
    Input_listener::staticConstructor(window, MOUSE_SENSITIVITY, MAX_DISTANCE_REMOVE);
    Camera::staticConstructor();

    Shader shader_color(path_string + "vertex_shader_color.txt", path_string + "fragment_shader_color.txt");

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
    
    // Render loop
    int frame_nb = 0;
    while (!glfwWindowShouldClose(window)){
        frame_nb++;
        map.check_remove_add_cube(); // Check if the input listener wrote that we should remove or add cubes, and remove or add them if needed

        // Checks if a key is being pressed
        Input_listener::processInput(window, deltaTime);

        double current_time = fps(); // Calculates and prints FPS

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

        // View: move world view on camera space
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(Camera::camera_pos, Camera::camera_pos+Camera::camera_front, Camera::movement_up);
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it

        // Projection: project 3D view on 2D
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

        cubemap.draw_skybox(VAO_skybox, cubemap.cubemap_ID, view, projection, current_time, shader_skybox); // current_time used to blend day color and night texture during morning and evening
        map.draw_cubes(view, projection);
        axis.draw_axis(VAO_axis, view, projection, shader_color);
        target.draw_target(VAO_target, shader_color);

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen
    }
    // De-allocate
    glDeleteVertexArrays(1, &VAO_axis);
    glDeleteVertexArrays(1, &VAO_target);

    glfwTerminate(); // Clean GLFW resources
    return 0;
}


