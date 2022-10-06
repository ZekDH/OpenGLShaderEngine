#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


void theme(){
    //auto& style = ImGui::GetStyle();
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

int main()
{
    std::cout << "MainCalled" << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    //glfw --> way to uncap frameRate (need to figure out deltaTime)
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "initShaderMain" << std::endl;
    shader basicShader("Fbm.shader");
    glUseProgram(basicShader.shaderProgram);

    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        1.0f,  1.0f, 0.0f,  // top right
        1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    }; 

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float time = 0.0;
    float timeAdd = 0.01;

    //Setup IMGUI
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    ImVec4 BaseColour;
    ImVec4 StreakColour;
    ImVec4 Colour1;
    ImVec4 Colour2;
    ImVec4 Colour3;
    ImVec4 Colour4;
    theme();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        // Feed inputs to dear imgui, start new frame
        ImGui::NewFrame();
        
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth   = ImGui::CalcTextSize("Debug Menu For Shader Uniforms").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("Debug Menu For Shader Uniforms");
        textWidth   = ImGui::CalcTextSize("Application average %.3f ms/frame (%.1f FPS)").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::DragFloat("TimeScale", &timeAdd, 0.005f, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::ColorEdit3("BaseColour", (float*)&BaseColour);
        ImGui::ColorEdit3("StreakColour", (float*)&StreakColour);
        ImGui::ColorEdit3("Col1", (float*)&Colour1);
        ImGui::ColorEdit3("Col2", (float*)&Colour2);
        ImGui::ColorEdit3("Col3", (float*)&Colour3);
        ImGui::ColorEdit3("Col4", (float*)&Colour4);
        

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // input
        // -----
        processInput(window);

        // render
        // ------
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


        time += timeAdd;

        // draw our first triangle
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(basicShader.shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

