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
#include "Shadow.h"
#include "Particles.h"
#include "Model.h"

#define PATH "../../Project/" // Path to go from where the program is run to current folder
#define MOUSE_SENSITIVITY 0.05 // Sensitivity of yaw and pitch wrt mouse movements
#define MAX_DISTANCE_REMOVE 15 // We only remove clicked blocks up to this distance
#define NUM_CUBES_SIDE 100 // We create a NUM_CUBES_SIDE x NUM_CUBES_SIDE area of cubes
#define DAY_DURATION 200000 // Nb of milliseconds in an in-game day
#define NEAR 0.1f
#define FAR 100.0f // Near and far values used for perspective projection
#define CAMERA_SPEED 6.0f // Speed of movement of camera
#define MIRROR_RESOL 1000 // Resolution of mirrors
#define SHADOW_DEPTH_SIZE 4096 // Size of the depth map frame (larger means more rays)
#define SPEED_RAINFALL 3 // Speed of fall of the rain drops
#define AREA_RAIN_DROPS 15 // Rain appears in a AREA_RAIN_DROPS x AREA_RAIN_DROPS zone around the camera
#define NUMBER_RAIN_DROPS 8000 // Number of rain drops in the defined area
#define SUNNY false // Whether we want the weather to be sunny (sun and shadows) or rainy

int width = 1600, height = 1000; // Size of screen
std::vector<std::string> files_textures = {"grass.png", "dirt.png", "gold.png", "spruce.png", "bookshelf.png", "leaf.png", "glass.png"};
std::vector<float> textures_shininess = {60.0f, 75.0f, 4.0f, 50.0f, 50.0f, 40.0f, 10.0f}; // Shininess (amount of specular light reflected) respectively for each of the textures of files_textures
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
        Texture texture(path_string + "Textures/" + files_textures[i], textures_shininess[i], opaque, glm::vec3(0.0f), glm::vec3(0.0f), Mirror::resolution); // Previous-to-last 2 arguments are not used for non-mirror textures
    }

    // Create all relevant objects
    Cubemap cubemap(path_string, SUNNY);
    Map map(NUM_CUBES_SIDE, path_string);
    Input_listener::staticConstructor(window);
    Camera camera(CAMERA_SPEED);
    Axis axis(path_string);
    Target target(path_string);
    Sun sun(path_string, original_light_color, distance_sun_to_origin);
    Mirror::resolution = MIRROR_RESOL; // Set mirror resolutions
    Particles particles(path_string, camera.camera_pos, SPEED_RAINFALL, NUMBER_RAIN_DROPS, AREA_RAIN_DROPS);
    Model ourModel(path_string + "backpack/backpack.obj");
    Shader shaderModel(path_string + "vertex_shader_model.txt", path_string + "fragment_shader_model.txt");
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing to know which triangles are more in front
    glEnable(GL_STENCIL_TEST); //Enable stencil testing to draw the borders of the mirrors
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // do nothing if depth or stenicl test fails, if both succeed, replace the stored stencil value with the reference value
    glStencilMask(0x00); //prevent of writing in the stencil buffer
    Shadow::init_depth_map_framebuffer(SHADOW_DEPTH_SIZE, SHADOW_DEPTH_SIZE);
    Shader shadow_shader(path_string + "vertex_shader_shadow.txt", path_string + "fragment_shader_shadow.txt");
    glm::mat4 projection_light = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 50.0f, 150.0f); // Need larger frustum otherwise shadows won't be computed far from the sun, aspect is 1 since depth map is 1024x1024
    shadow_shader.use();
    shadow_shader.set_uniform("projection", projection_light);
    sun.projection_light = projection_light;

    // Render loop
    int frame_nb = 0;
    while (!glfwWindowShouldClose(window)){
        frame_nb++;
        std::cout << "FPS: " << fps() << std::endl;

        // First pass is for computing the shadows
        glm::mat4 view_light = glm::lookAt(sun.light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // View from the sun towards the center of the map
        shadow_shader.use();
        shadow_shader.set_uniform("view", view_light);
        sun.view_light = view_light;

        glViewport(0, 0, Shadow::shadow_width, Shadow::shadow_height);
        glBindFramebuffer(GL_FRAMEBUFFER, Shadow::depth_map_framebuffer);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear stencil and depth buffers
        Shadow::draw_objects_with_shadow(map.cubes, projection_light, view_light, shadow_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::width, Window::height);

        // Passes on all the framebuffers to generate the mirror images
        for (Texture texture: Texture::textures){
            if (!texture.mirror) continue; // If the texture is not mirror we don't have to compute the view

            glBindFramebuffer(GL_FRAMEBUFFER, texture.framebuffer);
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set color to use when clearing
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ); // Clear color stencil and depth buffers

            // Calculate view and projection matrices
            glm::vec3 mirror_position = texture.position;
            glm::vec3 incident = glm::normalize(mirror_position-camera.camera_pos);
            glm::vec3 normal_mirror = texture.direction;
            glm::vec3 reflected = glm::reflect(incident, normal_mirror);
            glm::vec3 up;
            if (length(normal_mirror-glm::vec3(0.0f, 1.0f, 0.0f)) < 0.01f) up = glm::vec3(0.0f, 0.0f, 1.0f); // If mirror is on the ground, up is in this direction to make the image in the mirror in the correct direction
            else  if (length(normal_mirror-glm::vec3(0.0f, -1.0f, 0.0f)) < 0.01f) up = glm::vec3(0.0f, 0.0f, 1.0f);
            else up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 view = glm::lookAt(mirror_position, mirror_position+reflected, up); // View: move world view on camera space
            // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
            float fov = 2.0f * atanf ((1.0f/2.0f)/glm::length(mirror_position-camera.camera_pos)); // Triangle formed by camera position, mirror center and mirror top, mirror size being 1
            glm::mat4 projection = glm::perspective(fov, 1.0f, Window::near, Window::far); // Projection: project 3D view on 2D

            // Draw Drawable objects that should be reflected in mirrors
            glViewport(0, 0, 1000, 1000);
            cubemap.draw_skybox(view, projection, glfwGetTime(), DAY_DURATION); // current_time used to blend day color and night texture during morning and evening
            //axis.draw_axis(view, projection);
            if (SUNNY) sun.draw_sun(view, projection, glfwGetTime(), DAY_DURATION, camera.camera_pos); // Give the camera position to draw the sun at distance 99 of the camera
            // Draw opaque cubes
            map.draw_opaque_cubes(view, projection, sun, camera.camera_pos); // Give the sun object to draw_cubes to let him read the sun color and position to draw light effectively
            // Draw mirrors and their borders
            glStencilFunc(GL_ALWAYS, 1, 0xFF); //all fragments of the mirrors should pass the test
            glStencilMask(0xFF); //allow writing in the stencil buffer
            Mirror::draw_mirrors(view, projection, sun, camera.camera_pos);
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //draw only fragments where mirror is not
            glStencilMask(0x00);    //prevents from writing in stencil buffer
            Mirror::draw_borders(view, projection, sun, camera.camera_pos); //draw upscaled versions of the mirrors to draw the border
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            // Draw particles of rain
            if (!SUNNY) particles.draw_particles(view, projection, camera.camera_pos);
            // Draw non opaque cubes
            map.draw_non_opaque_cubes(view, projection, sun, camera.camera_pos); // Draw transparant cubes last
            glViewport(0, 0, Window::width, Window::height);
        }

        // Last pass is to show on the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default FBO
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);

        // Update particles of rain positions
        particles.update_positions(delta_time, camera.camera_pos);

        // Calculate view and projection matrices
        glm::mat4 view = glm::lookAt(camera.camera_pos, camera.camera_pos+camera.camera_front, camera.movement_up); // View: move world view on camera space
        // CameraFront is the direction from camera to object, so cameraPos+cameraFront is one of the points we are looking it
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, Window::near, Window::far); // Projection: project 3D view on 2D

        // Draw all Drawable objects
        cubemap.draw_skybox(view, projection, glfwGetTime(), DAY_DURATION); // current_time used to blend day color and night texture during morning and evening
        //axis.draw_axis(view, projection);
        if (SUNNY) sun.draw_sun(view, projection, glfwGetTime(), DAY_DURATION, camera.camera_pos); // Give the camera position to draw the sun at distance 99 of the camera
        // Draw opaque cubes
        glActiveTexture(GL_TEXTURE1); // Put the depth map of shadows in texture unit 1
        glBindTexture(GL_TEXTURE_2D, Shadow::depth_map);
        glActiveTexture(GL_TEXTURE0); // Go back to texture unit 0
        map.draw_opaque_cubes(view, projection, sun, camera.camera_pos); // Give the sun object to draw_cubes to let him read the sun color and position to draw light effectively
        // Draw mirrors and their borders
        glStencilFunc(GL_ALWAYS, 1, 0xFF); //all fragments of the mirrors should pass the test
        glStencilMask(0xFF); //allow writing in the stencil buffer
        Mirror::draw_mirrors(view, projection, sun, camera.camera_pos);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //draw only fragments where mirror is not
        glStencilMask(0x00);    //prevents from writing in stencil buffer
        Mirror::draw_borders(view, projection, sun, camera.camera_pos); //draw upscaled versions of the mirrors to draw the border
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        // Draw particles of rain
        if (!SUNNY) particles.draw_particles(view, projection, camera.camera_pos);
        // Draw non opaque cubes
        map.draw_non_opaque_cubes(view, projection, sun, camera.camera_pos); // Draw transparant cubes last
        target.draw_axis(); // Target drawn the latest to be in front of the rest (despite being drawn with depth mask at false)
        shaderModel.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shaderModel.set_uniform("model", model);
        ourModel.draw(shaderModel);
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


