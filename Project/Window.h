#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Window{
    public:
    static inline int height = 1000, width = 1600;
    static inline float near, far; // Near and far values used for perspective projection

    static GLFWwindow* init_window(float near, float far){
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

        Window::near = near;
        Window::far = far;

        return window;
    }

    static void loadWindow(GLFWwindow* window){
        // Load OpenGL functions thanks to GLAD
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) throw std::runtime_error("Failed to initialize GLAD");
        glfwGetFramebufferSize(window, &width, &height); // Get real width and height of window (might be different from what we asked for because of Retine displays)
        glViewport(0, 0, width, height); // Set window size to let OpenGL transform from normalized coordinates
        Window::width = width;
        Window::height = height;
    }
    
};

#endif