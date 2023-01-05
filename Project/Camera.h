#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Window.h"

class Camera{
public:
    float yaw, pitch;
    glm::vec3 camera_pos;
    glm::vec3 camera_front; // Direction from camera to object (z points to us)
    glm::vec3 movement_front; // Direction in which the user is moving when moving forward (i.e. taking into account yaw but not pitch)
    glm::vec3 movement_up;
    float camera_speed;

    Camera(float camera_speed){
        yaw = -90.0f;
        pitch = 0;

        camera_pos = glm::vec3(0.0f, 8.0f,  0.0f); // Initial position
        camera_front = glm::vec3(0.0f, 0.0f, -1.0f); // By convention z axis is facing towards us
        movement_front = glm::vec3(0.0f, 0.0f, -1.0f);
        movement_up = glm::vec3(0.0f, 1.0f,  0.0f);

        this->camera_speed = camera_speed;
    }

    void update_orientation(float variation_yaw, float variation_pitch) {
        yaw += variation_yaw;
        pitch += variation_pitch;
        if (pitch > 89.0f) pitch =  89.0f; // Limits for pitch
        if (pitch < -89.0f) pitch = -89.0f;
        update_camera_front();
    }

    glm::vec3 get_new_position(std::string direction, float deltaTime){
        const float camera_move = camera_speed * deltaTime; // Camera speed depends on time between frames => same apparent speed no matter the number of fps

        // Calculate variation of camera_pos according to direction of movement and camera_speed
        glm::vec3 variation_pos;
        if (direction == "front") variation_pos = movement_front * camera_move; // Move in front of camera
        else if (direction == "back") variation_pos = -movement_front * camera_move;
        else if (direction == "left") variation_pos = -glm::normalize(glm::cross(movement_front, movement_up)) * camera_move;
        else if (direction == "right") variation_pos = glm::normalize(glm::cross(movement_front, movement_up)) * camera_move;
        else if (direction == "up") variation_pos = movement_up * camera_move;
        else if (direction == "down") variation_pos = -movement_up * camera_move;
        else std::cerr << "Error: direction given to update_position is " << direction << std::endl;
        return camera_pos + variation_pos;
    }

    void update_position(glm::vec3 new_position){ // Called after get_new_position and checking that the new position is valid
        camera_pos = new_position;
    }

    glm::vec4 unproject_2D_coord(){
         // Un-projects the 2D cordinate corresponding to the middle of the screen towards its 3D position
         // First 3 components of output is the 3D object coordinates, and 4th is its depth
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

private:
    void update_camera_front(){ // Update camera_front according to yaw and pitch
        glm::vec3 direction_yaw_pitch; // Take into account both the pitch and the yaw for camera orientation
        direction_yaw_pitch.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction_yaw_pitch.y = sin(glm::radians(pitch));
        direction_yaw_pitch.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_front = glm::normalize(direction_yaw_pitch);

        glm::vec3 direction_yaw; // Take into account only the yaw for user movement
        direction_yaw.x = cos(glm::radians(yaw));
        direction_yaw.y = 0;
        direction_yaw.z = sin(glm::radians(yaw));
        movement_front = glm::normalize(direction_yaw);
    }
};

#endif