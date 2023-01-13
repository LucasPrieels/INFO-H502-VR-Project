#ifndef SHADOW_H
#define SHADOW_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Drawable.h"

class Shadow: public Drawable{
public:
    static inline int shadow_width, shadow_height;
    static inline unsigned int depth_map_framebuffer, depth_map;

    static void init_depth_map_framebuffer(int shadow_width, int shadow_height){
        Shadow::shadow_width = shadow_width;
        Shadow::shadow_height = shadow_height;

        // Create framebuffer
        glGenFramebuffers(1, &depth_map_framebuffer);

        // Create depth map as a 2D texture with the specified size
        glGenTextures(1, &depth_map);
        glBindTexture(GL_TEXTURE_2D, depth_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // Outside the size of the depth map, specify that there should be not shadow by putting white borders
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float color_border[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color_border);

        // Associate the depth map with the depth map framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
        glDrawBuffer(GL_NONE); // Should not write or read in the color buffer because we are only interested in the depth
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    static void draw_objects_with_shadow(std::vector<Cube> cubes, glm::mat4 projection_light, glm::mat4 view_light, Shader shadow_shader){
        // Draw all Drawable objects that should have a shadow, i.e. blocks (mirrors, sun, target, axis etc should not have shadows)
        Drawable drawable(Cube::vertices, true, Cube::vertices_indices, {3}); // Create VAO
        std::vector<glm::vec3> translation_cubes;
        for (Cube c: cubes) translation_cubes.push_back(glm::vec3(c.x, c.y, c.z));

        drawable.draw(translation_cubes, view_light, projection_light, shadow_shader, 1, 36, GL_TRIANGLES, true, false);
    }
};
#endif