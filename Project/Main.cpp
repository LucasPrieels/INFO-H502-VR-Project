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
#define CAMERA_SPEED 6.0f // Speed of movement of camera

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

void check_for_input(GLFWwindow* window, Camera* camera, Map* map){
    // Key pressing
    std::vector<std::string> directions = Input_listener::processInput(window); // Returns the list of directions linked to keys pressed by user (if ESC is pressed, directly asks OpenGL to close)
    for (int i = 0; i < directions.size(); i++){
        glm::vec3 new_position = camera->get_new_position(directions[i], delta_time/sqrt(directions.size()));
        // Without correction /sqrt(directions.size()), we are going faster when moving in 2 directions at the same time (e.g. front and
        // right), because we are going at set speed in each direction, so speed*sqrt(2) in total

        // Only update position if position is valid (i.e. not inside any of the cubes)
        if (map->part_of_cubes(new_position)) camera->update_position(new_position);

    }
    // Left or right clicks
    if (Input_listener::left_click){
        glm::vec4 unprojected = camera->unproject_2D_coord();
        float depth = unprojected.w;
        if (depth < MAX_DISTANCE_REMOVE) map->check_remove_cube(glm::vec3(unprojected)); // Remove block corresponding to this position
        Input_listener::left_click = false;
    }
    else if (Input_listener::right_click){ // Can't have both a left and a right click on the same frame
        glm::vec4 unprojected = camera->unproject_2D_coord();
        float depth = unprojected.w;
        if (depth < MAX_DISTANCE_REMOVE) map->check_add_cube(glm::vec3(unprojected)); // Remove block corresponding to this position
        Input_listener::right_click = false;
    }
    // Mouse movement
    if (Input_listener::mouse_movement_x != 0 || Input_listener::mouse_movement_y != 0){
        float variation_yaw = Input_listener::mouse_movement_x * MOUSE_SENSITIVITY; // Update yaw and pitch with the mouse movement according to a certain sensitivity
        float variation_pitch = Input_listener::mouse_movement_y * MOUSE_SENSITIVITY;
        camera->update_orientation(variation_yaw, variation_pitch);
        Input_listener::mouse_movement_x = 0;
        Input_listener::mouse_movement_y = 0;
    }
}

int main(int argc, char* argv[]){
    GLFWwindow* window = Window::init_window(NEAR, FAR);
    Window::loadWindow(window);

    // Create all relevant objects
    Map map(NUM_CUBES_SIDE, path_string);
    Input_listener::staticConstructor(window);
    Camera camera(CAMERA_SPEED);
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

        // Calculate view and projection matrices
        glm::mat4 view = glm::lookAt(camera.camera_pos, camera.camera_pos+camera.camera_front, camera.movement_up); // View: move world view on camera space
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, Window::near, Window::far); // Projection: project 3D view on 2D

        // Draw all Drawable objects
        cubemap.draw_skybox(view, projection, glfwGetTime(), DAY_DURATION); // current_time used to blend day color and night texture during morning and evening
        axis.draw_axis(view, projection);
        map.draw_cubes(view, projection);
        target.draw_axis();

        // Checks for inputs signaled by Input_listener (button clicked, mouse clicked or mouse moved)
        check_for_input(window, &camera, &map);

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen
    }

    glfwTerminate(); // Clean GLFW resources
    return 0;
}


