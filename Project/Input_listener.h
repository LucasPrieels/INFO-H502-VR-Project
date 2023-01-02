#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Camera.h"

 class Input_listener{
public:
    Input_listener() {
    }

    static void staticConstructor(GLFWwindow* window){
        // Window resizing
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Mouse input
        Input_listener::first_call_mouse = true;
        Input_listener::mouse_sensitivity = 0.05; // Sensitivity of yaw and pitch change according to mouse movement
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture cursor and hide it
        glfwSetCursorPosCallback(window, mouse_callback); // Set callback when mouse moves
    }
private:
    static inline bool first_call_mouse; // Stores whether it is the first callback for mouse (needed to initilize last_mouse_pos_x and last_mouse_pos_y)
    // TODO inline is C++17, are we allowed ?
    static inline int last_mouse_pos_x, last_mouse_pos_y; // Stores last position (x,y) of mouse
    static inline float mouse_sensitivity; // Sensitivity of yaw and pitch change according to mouse movement

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height){ // Called when the window is resized
        std::cout << "Resized window to width=" << width << " height=" << height << std::endl;
        glViewport(0, 0, width, height); // Does this command do anything ? It works as well without
    }

    static void mouse_callback(GLFWwindow* window, double mouse_pos_x, double mouse_pos_y){
        if (first_call_mouse){
            last_mouse_pos_x = mouse_pos_x;
            last_mouse_pos_y = mouse_pos_y;
            first_call_mouse = false;
        }

        float mouse_movement_x = mouse_pos_x - last_mouse_pos_x;
        float mouse_movement_y = -(mouse_pos_y - last_mouse_pos_y); // Minus in front because y axis on screen is defined to the bottom (TODO check if is it true)
        last_mouse_pos_x = mouse_pos_x;
        last_mouse_pos_y = mouse_pos_y;


        float variation_yaw = mouse_movement_x * mouse_sensitivity; // Update yaw and pitch with the mouse movement according to a certain sensitivity
        float variation_pitch = mouse_movement_y * mouse_sensitivity;

        Camera::update_orientation(variation_yaw, variation_pitch);
    }
};

#endif