project("Project")

#Put the sources into a variable
set(SOURCE "Main.cpp" "Camera.h" "Shader.h" "Input_listener.h" "stb_image.h" "Texture.h" "Cubemap.h" "Cube.h" "Axis.h" "Window.h" "Target.h" "Drawable.h" "Map.h" "Sun.h" "Mirror.h")


#To use the content of a variable you need to use ${NAME_OF_YOUR_VARIABLE}
#Specify that you want to generate an executable with a certain name using a set of sources
add_executable(${PROJECT_NAME}_v1 ${SOURCE})
#Specify which libraries you want to use with your executable
target_link_libraries(${PROJECT_NAME}_v1 PUBLIC OpenGL::GL glfw glad)