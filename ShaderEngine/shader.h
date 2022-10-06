#ifndef SHADER_H
#define SHADER_H
#include <iostream>

struct ShaderSource{
    std::string vertexShader;
    std::string fragmentShader;
};

class shader{

    public:
        
        unsigned int shaderProgram;
        std::string filepath;
        ShaderSource parseShader(const std::string filepath);
        unsigned int initalizeShader(const std::string filepath);

        shader(std::string inputFilepath){
            filepath = inputFilepath;
            shaderProgram = initalizeShader(filepath);
        }
};
 
#endif