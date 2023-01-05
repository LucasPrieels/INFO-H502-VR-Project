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
#define MOUSE_SENSITIVITY 0.05 // Sensitivity of yaw and pitch wrt mouse movements
#define MAX_DISTANCE_REMOVE 15 // We only remove clicked blocks up to this distance
#define NUM_CUBES_SIDE 80 // We create a NUM_CUBES_SIDE x NUM_CUBES_SIDE area of cubes
#define DAY_DURATION 200000 // Nb of milliseconds in an in-game day
#define NEAR 0.1f
#define FAR 100.0f // Near and far values used for perspective projection

 int width = 1600, height = 1000; // Size of screen
float lastFrame = 0.0f; // Time of last frame
float delta_time = 0.0f; // Time between 2 last frames
std::string path_string = PATH;

double fps(){
    // Calculates and prints FPS
    double current_time = glfwGetTime(); // Time in s since beginning of code running
    delta_time = current_time-lastFrame;
    double fps = 1/delta_time;
    lastFrame = current_time;
    return fps;
}

int main(int argc, char* argv[]){
    GLFWwindow* window = Window::init_window(NEAR, FAR);
    Window::loadWindow(window);

    // Create all relevant objects
    Map map(NUM_CUBES_SIDE, path_string);
    Input_listener::staticConstructor(window, MOUSE_SENSITIVITY, MAX_DISTANCE_REMOVE);
    Camera::staticConstructor();
    Axis axis(path_string);
    Target target(path_string);
    Cubemap cubemap(path_string);

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing to know which triangles are more in front
    
    // Render loop
    int frame_nb = 0;
    while (!glfwWindowShouldClose(window)){
        frame_nb++;
        std::cout << "FPS: " << fps() << std::endl;

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

        // Regular updates
        Input_listener::processInput(window, delta_time); // Checks if a key is being pressed
        map.check_remove_add_cube(); // Check if the input listener wrote that we should remove or add cubes, and remove or add them if needed

        // Calculate view and projection matrices
        glm::mat4 view = glm::lookAt(Camera::camera_pos, Camera::camera_pos+Camera::camera_front, Camera::movement_up); // View: move world view on camera space
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, Window::near, Window::far); // Projection: project 3D view on 2D

        // Draw all Drawable objects
        cubemap.draw_skybox(view, projection, glfwGetTime(), DAY_DURATION); // current_time used to blend day color and night texture during morning and evening
        map.draw_cubes(view, projection);
        axis.draw_axis(view, projection);
        target.draw_axis();

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen
    }

    glfwTerminate(); // Clean GLFW resources
    return 0;
}


