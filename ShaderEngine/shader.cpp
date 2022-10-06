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

    //Seperation of shader text as i am not splitting the vertex shader and fragment shader within this project
    enum class ShaderType{
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    //Shader initialization with shadertype none
    std::string line;
    ShaderType type = ShaderType::NONE;

    //File read check
    if(stream.fail()){
        std::cout << "Shader File Read Fail\n";
    }

    //Parsing the file and using line.find() to split the text into respective Vertex and Fragment shaders
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

    //Returning ShaderSource struct
    return {shaderStringstream[0].str(), shaderStringstream[1].str()};
}

unsigned int shader::initalizeShader(const std::string filepath){
    std::cout << "initShaderCalled" << std::endl;
    ShaderSource thisShader = parseShader(filepath);

    //Parsing the shader text from string to a character array (C string)
    std::cout << "parseShaderComplete" << std::endl;
    const char *vertexShaderSource = thisShader.vertexShader.c_str();
    const char *fragmentShaderSource = thisShader.fragmentShader.c_str();

    //Allocating the shader to the OpenGL program for use, and storing allocated shader within unsigned int
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //Variables used for shader compilation check's, with debug info storage
    int success;
    char infoLog[512];

    //Check for Vertex shader
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //Fragment shader initalization 
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //Check for fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //link shaders to OpenGL as a program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Additonal error check for program creation
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    //Deallocating storage within OpenGL of unused shader information, now the shader program is created
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "initShaderComplete" << std::endl;
    return shaderProgram;
}
