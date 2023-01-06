#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string

class Input_listener{
public:
    static inline bool left_click = false, right_click = false; // Stores whether a left or a right click has been detected
    static inline float mouse_movement_x = 0, mouse_movement_y = 0;
    Input_listener() = default;

    static void staticConstructor(GLFWwindow* window){
        // Window resizing
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Mouse input
        Input_listener::first_call_mouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture cursor and hide it
        glfwSetCursorPosCallback(window, mouse_callback); // Set callback when mouse moves
        glfwSetMouseButtonCallback(window, mouse_button_callback); // Set callback on mouse click

        // Selected block is initially #1 (dirt)
        currently_selected_block = 1;
    }

     static std::vector<std::string> process_movement_input(GLFWwindow* window){
         // Called at each frame to check whether a key is pressed
         if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true); // Close the window if esc key is pressed

         std::vector<std::string> directions;
         if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) directions.push_back("front"); // Z on AZERTY keyboard
         if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) directions.push_back("back");
         if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) directions.push_back("left"); // Q on AZERTY keyboard
         if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) directions.push_back("right");
         if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) directions.push_back("up");
         if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) directions.push_back("down");

         return directions;
     }

     static int process_block_selection_input(GLFWwindow* window){
         if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currently_selected_block = 1;
         else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) currently_selected_block = 2;
         else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) currently_selected_block = 3;
         else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) currently_selected_block = 4;
         else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) currently_selected_block = 5;
         else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) currently_selected_block = 6;
         else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) currently_selected_block = 7;

         return currently_selected_block;
    }

private:
    static inline bool first_call_mouse; // Stores whether it is the first callback for mouse (needed to initilize last_mouse_pos_x and last_mouse_pos_y)
    static inline int last_mouse_pos_x, last_mouse_pos_y; // Stores last position (x,y) of mouse
    static inline int currently_selected_block; // Index of currently selected block (changes when user clicks a number on the keyboard)

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

        Input_listener::mouse_movement_x += mouse_pos_x - last_mouse_pos_x;
        Input_listener::mouse_movement_y += -(mouse_pos_y - last_mouse_pos_y); // Minus in front because y axis on screen is defined towards the bottom
        last_mouse_pos_x = mouse_pos_x;
        last_mouse_pos_y = mouse_pos_y;
    }

     // Callback on click on the mouse
     static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
         if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){ // On left click, remove block
             Input_listener::left_click = true;
         }

         else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){ // On right click, add block
             Input_listener::right_click = true;
         }
     }
};

#endif