#include <iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Input_listener.h"
#include "Camera.h"

#define PATH "../../Project/" // Path to go from where the program is run to current folder

const int width = 800;
const int height = 500;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { // If esc key has been pressed
        glfwSetWindowShouldClose(window, true); // Close the window
    }
    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Camera::update_position(cameraSpeed * Camera::camera_front);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Camera::update_position(-cameraSpeed * Camera::camera_front);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Camera::update_position(-glm::normalize(glm::cross(Camera::camera_front, Camera::camera_up)) * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Camera::update_position(glm::normalize(glm::cross(Camera::camera_front, Camera::camera_up)) * cameraSpeed);
}

int main(int argc, char* argv[]){
    //Boilerplate
    //1.a Create the OpenGL context
    if (!glfwInit()) throw std::runtime_error("Failed to initialise GLFW \n");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Use version 4.0 of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use Core version of OpenGL
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //1.b Create the window
    // Create window object
    GLFWwindow* window = glfwCreateWindow(width, height, "VR Project", nullptr, nullptr); // Seems that the window doesn't take the specified size TODO
    if (window == NULL){
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }
    glfwMakeContextCurrent(window);

    //load openGL function
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) throw std::runtime_error("Failed to initialize GLAD");
    glViewport(0, 0, width, height);
    Input_listener::staticConstructor(window);
    Camera::staticConstructor();

    float vertices[] = {
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f
    };

    unsigned int indices[] = {
            // Side x=0.5
            0, 1, 2, // first triangle
            1, 2, 3,  // second triangle
            // Side x=-0.5
            4, 5, 6,
            5, 6, 7,
            // Side y=-0.5
            2, 3, 6,
            3, 6, 7,
            // Side y=-0.5
            0, 1, 4,
            1, 4, 5,
            // Side z=-0.5
            0, 2, 4,
            2, 4, 6,
            // Side z=-0.5
            1, 3, 5,
            3, 5, 7
    };

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Same for color (not in the same line !)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float))); // Careful last argument is not only 3
    glEnableVertexAttribArray(1);

    // Deactivate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind the VBO and then VAO otherwise the VAO will store the unbinding of VBO
    glBindVertexArray(0);

    std::string path_string = PATH;
    Shader shader(path_string + "vertex_shader.txt", path_string + "fragment_shader.txt");

    //sync with the screen refresh rate
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing to know which triangles are more in front
    // 5. Your code: Rendering
    while (!glfwWindowShouldClose(window)) { // Render loop
        processInput(window); // Checks if esc key has been pressed

        // Calculates and prints FPS
        double current_time = glfwGetTime(); // Time in s since beginning of code running
        deltaTime = current_time-lastFrame;
        double fps = 1/deltaTime;
        lastFrame = current_time;
        std::cout << "FPS: " << fps << std::endl;

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        shader.use();

        glBindVertexArray(VAO);
        for (int i = 0; i < 10; i++){
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-10*(float)current_time), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 view = glm::mat4(1.0f);
            // note that we're translating the scene in the reverse direction of where we want to move
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            view = glm::lookAt(Camera::camera_pos, Camera::camera_pos+Camera::camera_front, Camera::camera_up); // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
            std::cout << glm::to_string(Camera::camera_pos) << " " << glm::to_string(Camera::camera_front) << " " << glm::to_string(Camera::camera_up) << std::endl;
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);


            shader.set_uniform("model", model);
            shader.set_uniform("view", view);
            shader.set_uniform("projection", projection);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 3 vertices per triangle, 2 triangles per side, 6 sides per cube
        }

        glfwPollEvents(); // Checks if an event has been triggered, and if needed calls the corresponding callback
        glfwSwapBuffers(window); // Shows rendering buffer on the screen
    }

    glfwTerminate(); // Clean GLFW ressources
    return 0;
}
