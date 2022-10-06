#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"


//Declaring functions before they are implemented later within the program
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

//Inital screen size variables that are used
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Custom theme for ImGui
void theme(){
    ImGuiIO& io = ImGui::GetIO();
    ImVec4* colors = ImGui::GetStyle().Colors;

    io.Fonts->AddFontFromFileTTF("Font/UbuntuMono-R.ttf", 14.5f);

    ImGui::GetStyle().FrameBorderSize = 1.0f;
    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;
    ImGui::GetStyle().WindowRounding = 7.0f;

    colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.7f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.1f, 0.1f, 0.1f, 0.7f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
}

int main(){
    std::cout << "MainCalled" << std::endl;


    //GLFW INITIALIZE
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    //Basic window intialization for GLFW from OpenGL tutorial resources
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shader Enginge", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    //Using my shader script the loads the shader program for openGL to use
    //The basicShader function is called with the shader file location
    std::cout << "initShaderMain" << std::endl;
    shader basicShader("Fbm.shader");

    //glUseProgram is called to initalize the shader within OpenGL so it can be used within the render method
    //This could be done within the render loop as well, but i am initializing the shader above to save resources as i do not need to change shader during runtime
    glUseProgram(basicShader.shaderProgram);

    

    //Vertex buffer is used to define the vertices within the triangle, there are four vertices because i will be creating a mesh of a square
    //Third point is the Z axis i am not using the Z axis so i have left it at zero
    float vertices[] = {
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    //Unsigned int is used because indices cannot be above -1
    //Indices are used to share vertices, to stop two triangles and 6 vertices from being created to make the square
    //Instead two triangles and 4 vertices are used, vertices 1 and 3 are shared by both triangles
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    }; 

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    //Vertex buffer is allocated for storage
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Shared vertices link information is allocated for storage
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    //This is used to show the wireframe of the mesh, this command fills in the triangles and displays the material on both sides
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Time is the starting time for the shader, timeAdd changes how quickly the shader moves
    unsigned float time = 0.0;
    unsigned float timeAdd = 0.01;

    //Setup IMGUI
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    //Me setting up the theme and variables i am going to use with ImGui
    ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    ImVec4 BaseColour;
    ImVec4 StreakColour;
    ImVec4 Colour1;
    ImVec4 Colour2;
    ImVec4 Colour3;
    ImVec4 Colour4;
    theme();

    //Render loop
    while (!glfwWindowShouldClose(window)){

        //Updating graphical inputs each frame through the render loop
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        //Setup for ImGui, showing FPS and displaying title of context
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize("Debug Menu For Shader Uniforms").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("Debug Menu For Shader Uniforms");
        textWidth = ImGui::CalcTextSize("Application average %.3f ms/frame (%.1f FPS)").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::DragFloat("TimeScale", &timeAdd, 0.005f, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        //Uniform variables exposed to ImGui controls
        ImGui::ColorEdit3("BaseColour", (float*)&BaseColour);
        ImGui::ColorEdit3("StreakColour", (float*)&StreakColour);
        ImGui::ColorEdit3("Col1", (float*)&Colour1);
        ImGui::ColorEdit3("Col2", (float*)&Colour2);
        ImGui::ColorEdit3("Col3", (float*)&Colour3);
        ImGui::ColorEdit3("Col4", (float*)&Colour4);
        

        //Process to render ImGui to the screen after all updates to the frame are called
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        //Recieving input using a function that calls glfw key commands
        processInput(window);

        //Render to the screen, update the shader uniforms with implemented variables that are changed above with ImGui
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        int location = glGetUniformLocation(basicShader.shaderProgram, "iTime");
        glUniform1f(location,time);

        int res = glGetUniformLocation(basicShader.shaderProgram, "iResolution");
        int width = 0;
        int height = 0;
        glfwGetWindowSize(window, &width, &height);
        glUniform3f(res,width,height,0.0);

        int baseCol = glGetUniformLocation(basicShader.shaderProgram, "baseColour");
        glUniform3f(baseCol, BaseColour.x, BaseColour.y, BaseColour.z);

        int streakCol = glGetUniformLocation(basicShader.shaderProgram, "streakColour");
        glUniform3f(streakCol, StreakColour.x, StreakColour.y, StreakColour.z);

        int Col1 = glGetUniformLocation(basicShader.shaderProgram, "ColourOne");
        glUniform3f(Col1, Colour1.x, Colour1.y, Colour1.z);

        int Col2 = glGetUniformLocation(basicShader.shaderProgram, "ColourTwo");
        glUniform3f(Col2, Colour2.x, Colour2.y, Colour2.z);

        int Col3 = glGetUniformLocation(basicShader.shaderProgram, "ColourThree");
        glUniform3f(Col3, Colour3.x, Colour3.y, Colour3.z);

        int Col4 = glGetUniformLocation(basicShader.shaderProgram, "ColourFour");
        glUniform3f(Col4, Colour4.x, Colour4.y, Colour4.z);

        //Speed increase to the shader
        time += timeAdd;

        // drawing the square using the stored vertex information
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Update the ImGui method, after all edits are completed
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //Glfw check for key events at the end of render loop
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //This is used at the end to clear unused memory once render loop is completed
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(basicShader.shaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

