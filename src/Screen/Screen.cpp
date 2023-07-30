#include "Screen.hpp"

using namespace Explorer400D;

Screen::Screen()
{
    // Initialize the window with GLFW
    if (!glfwInit())
        return;
    this->_window = std::shared_ptr<GLFWwindow>(glfwCreateWindow(1600, 900, "Pitbox", NULL, NULL), glfwDestroyWindow);
    if (!this->_window)
        return;
    glfwMakeContextCurrent(this->_window.get());

    // Initialize ImGui with GLFW & OpenGL
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    this->windowLoop();
}

Screen::~Screen()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->_window.get());
    glfwTerminate();
}

void Screen::windowLoop()
{
    while (!glfwWindowShouldClose(this->_window.get())) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Dockspace
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Main menu bar
        this->mainMenuBar();

        // Render the window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the buffers
        glfwSwapBuffers(this->_window.get());
        glfwPollEvents();
    }
}

void Screen::mainMenuBar()
{
}