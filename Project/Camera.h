#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include "Shader.h"
#include "Input_listener.h"

 class Camera{
public:
    static inline float yaw, pitch;
    static inline glm::vec3 camera_pos;
    static inline glm::vec3 camera_front; // Direction from camera to object (z points to us)
    static inline glm::vec3 camera_up;

    Camera() = default; // Default constructor (we only use the static one)

    static void staticConstructor(){
        yaw = -90.0f;
        pitch = 0;

        camera_pos = glm::vec3(0.0f, 0.0f,  3.0f);
        camera_front = glm::vec3(0.0f, 0.0f, -1.0f); // By convention z axis is facing towards us
        camera_up = glm::vec3(0.0f, 1.0f,  0.0f);
    }

    static void update_orientation(float variation_yaw, float variation_pitch) {
        yaw += variation_yaw;
        pitch += variation_pitch;
        if (pitch > 89.0f) pitch =  89.0f; // Limits for pitch
        if (pitch < -89.0f) pitch = -89.0f;
        update_camera_front();
    }

    static void update_position(std::string direction, float deltaTime){
        const float camera_speed = 2.5f * deltaTime; // Camera speed depends on time between frames => same apparent speed no matter the number of fps

        // Calculate variation of camera_pos according to direction of movement and camera_speed
        glm::vec3 variation_pos;
        if (direction == "front") variation_pos = camera_speed * camera_front; // Move in front of camera
        else if (direction == "back") variation_pos = -camera_speed * camera_front;
        else if (direction == "left") variation_pos = -glm::normalize(glm::cross(Camera::camera_front, Camera::camera_up)) * camera_speed;
        else if (direction == "right") variation_pos = glm::normalize(glm::cross(Camera::camera_front, Camera::camera_up)) * camera_speed;
        else if (direction == "up") variation_pos = Camera::camera_up * camera_speed;
        else if (direction == "down") variation_pos = -Camera::camera_up * camera_speed;
        else std::cerr << "Error: direction given to update_position is " << direction << std::endl;
        camera_pos += variation_pos;
    }

private:
    static void update_camera_front(){ // Update camera_front according to yaw and pitch
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_front = glm::normalize(direction);
    }
};

#endif