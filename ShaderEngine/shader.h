#ifndef SHADER_H
#define SHADER_H
#include <iostream>

//String sources for each shader
struct ShaderSource{
    std::string vertexShader;
    std::string fragmentShader;
};

//Implemented shader class
class shader{

    public:
        unsigned int shaderProgram;
        std::string filepath;
        ShaderSource parseShader(const std::string filepath);
        unsigned int initalizeShader(const std::string filepath);

        //Shader initialzation function upon class creation
        shader(std::string inputFilepath){
            filepath = inputFilepath;
            shaderProgram = initalizeShader(filepath);
        }
};
 
#endif
