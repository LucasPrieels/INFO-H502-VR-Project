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

    Camera() {
    }

    static void staticConstructor(){
        yaw = -90.0f;
        pitch = 0;

        camera_pos = glm::vec3(0.0f, 0.0f,  3.0f);
        camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
        camera_up = glm::vec3(0.0f, 1.0f,  0.0f);
    }

    static void update_orientation(float variation_yaw, float variation_pitch) {
        std::cout << "Var: " << variation_yaw << " " << variation_pitch << std::endl;
        yaw += variation_yaw;

        pitch += variation_pitch;
        if (pitch > 89.0f) pitch =  89.0f; // Limits for pitch
        if (pitch < -89.0f) pitch = -89.0f;
        update_camera_front();
    }

    static void update_position(glm::vec3 variation_pos){
        camera_pos += variation_pos;
    }

private:
    static void update_camera_front(){
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_front = glm::normalize(direction);
    }
};

#endif