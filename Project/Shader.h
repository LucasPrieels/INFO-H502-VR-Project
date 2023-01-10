#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // for to_string
#include <fstream>

class Shader{
public:
    std::string vertex_shader_code, fragment_shader_code;
    std::string vertex_shader_path, fragment_shader_path;

    Shader(std::string vertex_shader_path, std::string fragment_shader_path){
        // Read then compile content of shader files

        vertex_shader_code = read_content_file(vertex_shader_path);
        fragment_shader_code = read_content_file(fragment_shader_path);

        GLuint vertex_shader = compileShader(vertex_shader_code, GL_VERTEX_SHADER);
        GLuint fragment_shader = compileShader(fragment_shader_code, GL_FRAGMENT_SHADER);
        program = compileProgram(vertex_shader, fragment_shader);

        this->vertex_shader_path = vertex_shader_path;
        this->fragment_shader_path = fragment_shader_path;
    }

    // Set mat4 uniform "name_uniform" with value given
    void set_uniform(std::string name_uniform, glm::mat4 value){
        unsigned int location = glGetUniformLocation(program, name_uniform.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    // Set int uniform "name_uniform" with value given
    void set_uniform(std::string name_uniform, int value){
        unsigned int location = glGetUniformLocation(program, name_uniform.c_str());
        glUniform1i(location, value);
    }

    // Set float uniform "name_uniform" with value given
    void set_uniform(std::string name_uniform, float value){
        unsigned int location = glGetUniformLocation(program, name_uniform.c_str());
        glUniform1f(location, value);
    }

    // Set glm::vec3 uniform "name_uniform" with value given
    void set_uniform(std::string name_uniform, glm::vec3 value){
        unsigned int location = glGetUniformLocation(program, name_uniform.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void use(){
        glUseProgram(program);
    }

private:
    GLuint program;

    std::string read_content_file(std::string path){
        std::ifstream stream(path);
        if (!stream) std::cerr << "There was an error opening file " << path << std::endl;

        std::string content = "";
        std::string line = "";
        while (std::getline(stream, line)){ // Read file line by line
            content += line;
            content += "\n";
        }

        stream.close();
        return content;
    }

    GLuint compileShader(std::string shaderCode, GLenum shaderType){
        GLuint shader = glCreateShader(shaderType);
        if (shader == 0) {
            std::cerr << "Error creating shader of type " << shaderType << std::endl;
            exit(EXIT_FAILURE);
        }

        // Compile shader
        const GLchar* codeArray[] = {shaderCode.c_str()};
        glShaderSource(shader, 1, codeArray, NULL);
        glCompileShader(shader);

        // Check if compilation was successful, otherwise prints error log
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            std::cerr << "Shader compilation failed!" << std::endl;

            // Get and print the info log
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Shader log: " << std::endl << infoLog << std::endl << std::endl;
        }
        return shader;
    }

    GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader){
        // Attach and link shaders to a new program
        GLuint programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        glLinkProgram(programID);

        // Check if compilation was successful, otherwise prints error log
        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
            std::cerr << "Program compilation failed!" << std::endl;

            // Get and print the info log
            char infoLog[512];
            glGetProgramInfoLog(programID, 512, NULL, infoLog);
            std::cerr << "Shader log: " << std::endl << infoLog << std::endl << std::endl;
        }

        // Shaders are not useful anymore so delete them
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return programID;
    }
};

#endif