#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "shader.h"


ShaderSource shader::parseShader(const std::string filepath){
    std::ifstream stream(filepath);
    std::stringstream shaderStringstream[2];

    enum class ShaderType{
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    ShaderType type = ShaderType::NONE;

    if(stream.fail()){
        std::cout << "Shader File Read Fail\n";
    }

    while(getline(stream, line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos){
                //Vertex shader selection
                type = ShaderType::VERTEX;
            }else if(line.find("fragment") != std::string::npos){
                //Fragment shader selection
                type = ShaderType::FRAGMENT;
            }
        }else{
            shaderStringstream[(int)type] << line << "\n";
        }
    }

    return {shaderStringstream[0].str(), shaderStringstream[1].str()};
}

unsigned int shader::initalizeShader(const std::string filepath){
    std::cout << "initShaderCalled" << std::endl;
    ShaderSource thisShader = parseShader(filepath);

    std::cout << "parseShaderComplete" << std::endl;
    const char *vertexShaderSource = thisShader.vertexShader.c_str();
    const char *fragmentShaderSource = thisShader.fragmentShader.c_str();


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    std::cout << "initShaderComplete" << std::endl;

    return shaderProgram;
}