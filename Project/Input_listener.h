#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Camera.h"
#include "Window.h"

class Input_listener{
public:
    static inline std::vector<glm::vec3> positions_to_remove, positions_to_add; // Stores positions clicked by the user to remove or add a block, regularly checked and emptied by class Map
    Input_listener() = default;

    static void staticConstructor(GLFWwindow* window, float mouse_sensitivity, float max_distance_remove){
        // Window resizing
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Mouse input
        Input_listener::first_call_mouse = true;
        Input_listener::mouse_sensitivity = mouse_sensitivity; // Sensitivity of yaw and pitch change according to mouse movement
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture cursor and hide it
        glfwSetCursorPosCallback(window, mouse_callback); // Set callback when mouse moves
        glfwSetMouseButtonCallback(window, mouse_button_callback); // Set callback on mouse click

        Input_listener::max_distance_remove = max_distance_remove; // We only remove (and add) clicked blocks up to this distance
    }

     static void processInput(GLFWwindow *window, float deltaTime){
         // Called at each frame to check whether a key is pressed
         if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true); // Close the window if esc key is pressed

         std::vector<std::string> directions;
         if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) directions.push_back("front"); // Z on AZERTY keyboard
         if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) directions.push_back("back");
         if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) directions.push_back("left"); // Q on AZERTY keyboard
         if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) directions.push_back("right");
         if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) directions.push_back("up");
         if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) directions.push_back("down");
         for (int i = 0; i < directions.size(); i++){
             Camera::update_position(directions[i], deltaTime/sqrt(directions.size()));
             // Without correction /sqrt(directions.size()), we are going faster when moving in 2 directions at the same time (e.g. front and
             // right), because we are going at set speed in each direction, so speed*sqrt(2) in total
         }
     }

private:
    static inline bool first_call_mouse; // Stores whether it is the first callback for mouse (needed to initilize last_mouse_pos_x and last_mouse_pos_y)
    static inline int last_mouse_pos_x, last_mouse_pos_y; // Stores last position (x,y) of mouse
    static inline float mouse_sensitivity; // Sensitivity of yaw and pitch change according to mouse movement
    static inline int max_distance_remove; // We only remove (and add) clicked blocks up to this distance

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height){ // Called when the window is resized
        std::cout << "Resized window to width=" << width << " height=" << height << std::endl;
        glfwGetFramebufferSize(window, &width, &height); // Get real width and height of window (might be different from what we asked for, for Retine displays for example)
        glViewport(0, 0, width, height);
    }

     // Update camera view according to mouse position
    static void mouse_callback(GLFWwindow* window, double mouse_pos_x, double mouse_pos_y){
        if (first_call_mouse){ // If it's the first time the function is called, don't update camera view but take current mouse position as initial one
            last_mouse_pos_x = mouse_pos_x;
            last_mouse_pos_y = mouse_pos_y;
            first_call_mouse = false;
        }

        float mouse_movement_x = mouse_pos_x - last_mouse_pos_x;
        float mouse_movement_y = -(mouse_pos_y - last_mouse_pos_y); // Minus in front because y axis on screen is defined towards the bottom
        last_mouse_pos_x = mouse_pos_x;
        last_mouse_pos_y = mouse_pos_y;

        float variation_yaw = mouse_movement_x * mouse_sensitivity; // Update yaw and pitch with the mouse movement according to a certain sensitivity
        float variation_pitch = mouse_movement_y * mouse_sensitivity;

        Camera::update_orientation(variation_yaw, variation_pitch);
    }

     // Callback on click on the mouse
     static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
         if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){ // On left click, remove block
            glm::vec4 unprojected = unproject_2D_coord(window);
             glm::vec3 object_coord = glm::vec3(unprojected.x, unprojected.y, unprojected.z);
             float depth_linear = unprojected.w;
            if (depth_linear < max_distance_remove) positions_to_remove.push_back(glm::vec3(object_coord.x, object_coord.y, object_coord.z));
         }

         else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){ // On right click, add block
             glm::vec4 unprojected = unproject_2D_coord(window);
             glm::vec3 object_coord = glm::vec3(unprojected.x, unprojected.y, unprojected.z);
             float depth_linear = unprojected.w;
             if (depth_linear < max_distance_remove) positions_to_add.push_back(glm::vec3(object_coord.x, object_coord.y, object_coord.z));
         }
     }

     static glm::vec4 unproject_2D_coord(GLFWwindow* window){ // First 3 components of output is the 3D object coordinates, and 4th is its depth
         glm::vec4 viewport = glm::vec4(0, 0, Window::width, Window::height);

         // Retrieve depth of clicked point
         float x = Window::width/2, y = Window::height/2; // We always click on the middle of the window
         float* data = (float*)calloc(1, sizeof(float));
         glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)data); // Get depth corresponding to clicked point
         float depth = *data; // Value from depth buffer is a non-linear depth (depth is stored more precisely when smaller)
         glm::vec3 screen_coord = glm::vec3(x, Window::height-y-1, depth);

         // Re-create view and projection matrices
         glm::mat4 view = glm::lookAt(Camera::camera_pos, Camera::camera_pos+Camera::camera_front, Camera::movement_up);
         glm::mat4 projection = glm::mat4(1.0f);
         float near = Window::near, far = Window::far;
         projection = glm::perspective(glm::radians(45.0f), (float)Window::width/(float)Window::height, near, far);

         // Use view and projection matrices to un-project clicked point and get their 3D coordinates
         glm::vec3 object_coord = glm::unProject(screen_coord, view, projection, viewport);
         float depth_linear = near*far/(far+depth*(near-far)); // Transform back the non-linear depth into a linear one
         // Formula used by OpenGL to compute the non-linear depth "depth" from linear one "z" is
         // depth = (1/z - 1/near)/(1/far - 1/near) (source: https://learnopengl.com/Advanced-OpenGL/Depth-testing)
         // Therefore the opposite formula is
         // z = (depth*(1/far-1/near)+1/near)^(-1) = (depth/far + (1-depth)/near)^(-1) = ((near*depth + far - far*depth)/(far*near))^(-1)
         // z = near*far/(far + depth*(near-far))
         return glm::vec4(object_coord, depth_linear);
    }
};

#endif