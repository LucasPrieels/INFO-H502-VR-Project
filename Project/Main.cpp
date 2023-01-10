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
#include "Sun.h"

#define PATH "../../Project/" // Path to go from where the program is run to current folder
#define MOUSE_SENSITIVITY 0.05 // Sensitivity of yaw and pitch wrt mouse movements
#define MAX_DISTANCE_REMOVE 15 // We only remove clicked blocks up to this distance
#define NUM_CUBES_SIDE 100 // We create a NUM_CUBES_SIDE x NUM_CUBES_SIDE area of cubes
#define DAY_DURATION 200000 // Nb of milliseconds in an in-game day
#define NEAR 0.1f
#define FAR 100.0f // Near and far values used for perspective projection
#define CAMERA_SPEED 6.0f // Speed of movement of camera

 int width = 1600, height = 1000; // Size of screen
 std::vector<std::string> files_textures = {"grass.png", "dirt.png", "gold.png", "spruce.png", "bookshelf.png", "leaf.png", "glass.png"};
std::vector<float> textures_shininess = {20.0f, 15.0f, 70.0f, 30.0f, 30.0f, 19.0f, 50.0f}; // Shininess (amount of specular light reflected) respectively for each of the textures of files_textures
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
    // Key pressing for movement
    std::vector<std::string> directions = Input_listener::process_movement_input(window); // Returns the list of directions linked to keys pressed by user (if ESC is pressed, directly asks OpenGL to close)
    for (int i = 0; i < directions.size(); i++){
        glm::vec3 new_position = camera->get_new_position(directions[i], delta_time/sqrt(directions.size()));
        // Without correction /sqrt(directions.size()), we are going faster when moving in 2 directions at the same time (e.g. front and
        // right), because we are going at set speed in each direction, so speed*sqrt(2) in total

        // Only update position if position is valid (i.e. not inside any of the cubes)
        if (map->part_of_cubes(new_position)) camera->update_position(new_position);
    }

    // Key pressing for selecting a block type
    int texture_num_selected = Input_listener::process_block_selection_input(window)-1; // -1 because the Input listener returns the selected key starting at 1 while textures ID start at 0

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
        if (depth < MAX_DISTANCE_REMOVE) map->add_cube(glm::vec3(unprojected), texture_num_selected, camera->camera_pos); // Add block (with the selected texture) corresponding to this position
        // Give the camera position to "add_cube" for the function to check whether the added cube is not too close to the camera
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

    // Light source properties
    glm::vec3 original_light_color(1.0f, 1.0f, 1.0f); // Color of the sun light originally (becomes more orange during sunrise and sunset)
    float distance_sun_to_origin = 99.0f; // Because above 100.0f objects are hidden by perspective projection

    // Load all possible block textures
    for (int i = 0; i < files_textures.size(); i++){
        bool opaque = !(files_textures[i] == "leaf.png" || files_textures[i] == "glass.png"); // Whether this texture is completely opaque or not. Only non-opaque textures are leaves and glass
        Texture texture(path_string + "Textures/" + files_textures[i], textures_shininess[i], opaque, glm::vec3(0.0f), glm::vec3(0.0f)); // Last 2 arguments are not used for non-mirror textures
    }

    // Create all relevant objects
    Cubemap cubemap(path_string);
    Map map(NUM_CUBES_SIDE, path_string);
    Input_listener::staticConstructor(window);
    Camera camera(CAMERA_SPEED);
    Axis axis(path_string);
    Target target(path_string);
    Sun sun(path_string, original_light_color, distance_sun_to_origin);

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing to know which triangles are more in front
    
    // Render loop
    int frame_nb = 0;
    while (!glfwWindowShouldClose(window)){
        frame_nb++;
        std::cout << "FPS: " << fps() << std::endl;

        // First passes on all the framebuffers to generate the mirror images
        for (Texture texture: Texture::textures){
            if (!texture.mirror) continue; // If the texture is not mirror we don't have to compute the view

            glBindFramebuffer(GL_FRAMEBUFFER, texture.framebuffer);
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

            // Calculate view and projection matrices
            glm::vec3 pos = texture.position;
            glm::vec3 front = texture.direction;
            glm::vec3 up;
            if (glm::length(front-glm::vec3(0.0f, 1.0f,  0.0f)) < 0.01f || glm::length(front-glm::vec3(0.0f, -1.0f,  0.0f)) < 0.01f) up = glm::vec3(0.0f, 0.0f, 1.0f);
            else up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 view = glm::lookAt(pos, pos+front, up); // View: move world view on camera space
            // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, Window::near, Window::far); // Projection: project 3D view on 2D

            // Draw all Drawable objects
            glViewport(0, 0, 500, 500);
            cubemap.draw_skybox(view, projection, glfwGetTime(), DAY_DURATION);
            axis.draw_axis(view, projection);
            sun.draw_sun(view, projection, glfwGetTime(), DAY_DURATION, pos);
            map.draw_cubes(view, projection, sun, pos); // Give pos and not the camera position otherwise the specular components changes when the camera moves !!
            Mirror::draw_mirrors(view, projection, sun, pos);
            glViewport(0, 0, Window::width, Window::height);
            // Don't write target
        }

        // Last pass is to show on the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default FBO
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate view and projection matrices
        glm::mat4 view = glm::lookAt(camera.camera_pos, camera.camera_pos+camera.camera_front, camera.movement_up); // View: move world view on camera space
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, Window::near, Window::far); // Projection: project 3D view on 2D

        // Draw all Drawable objects
        cubemap.draw_skybox(view, projection, glfwGetTime(), DAY_DURATION); // current_time used to blend day color and night texture during morning and evening
        axis.draw_axis(view, projection);
        sun.draw_sun(view, projection, glfwGetTime(), DAY_DURATION, camera.camera_pos); // Give the camera position to draw the sun at distance 99 of the camera
        map.draw_cubes(view, projection, sun, camera.camera_pos); // Give the sun object to draw_cubes to let him read the sun color and position to draw ligh effectively
        // Draw cubes after sun and axis because some cubes are transparent so they should be drawn after the opaque objects
        target.draw_axis(); // Target drawn the latest to be in front of the rest (despite being drawn with depth mask at false)
        Mirror::draw_mirrors(view, projection, sun, camera.camera_pos);

        // Checks for inputs signaled by Input_listener (button clicked, mouse clicked or mouse moved)
        check_for_input(window, &camera, &map);

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen

        // Check for potential errors
        GLenum err = glGetError();
        while(err != GL_NO_ERROR){
            std::cout << "### Error: 0x" << std::hex << err << std::endl;
            err = glGetError();
        }
    }

    glfwTerminate(); // Clean GLFW resources
    return 0;
}


