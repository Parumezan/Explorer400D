#include "Screen.hpp"

using namespace Explorer400D;

Screen::Screen(std::shared_ptr<Console> console) : _console(console), _cameraManager(console), _weather(console), _settings(console)
{
    // Initialize the window with GLFW
    if (!glfwInit())
        return;
    this->_window = std::shared_ptr<GLFWwindow>(glfwCreateWindow(1600, 900, "Explorer400D", NULL, NULL), glfwDestroyWindow);
    if (!this->_window)
        return;
    glfwMakeContextCurrent(this->_window.get());

    // Initialize ImGui with GLFW & OpenGL
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
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
    ImPlot::DestroyContext();
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
    if (this->_console->state)
        this->_console->frameLoop();
    if (this->_cameraManager.state)
        this->_cameraManager.frameLoop();
    if (this->_weather.state)
        this->_weather.frameLoop();
    if (this->_settings.state)
        this->_settings.frameLoop();

    // TODO: Integrate the event for actions (open, save, save as, exit)
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // if (ImGui::MenuItem("Open", "Ctrl+O")) {
            //     // TODO
            // }
            // if (ImGui::MenuItem("Save", "Ctrl+S")) {
            //     // TODO
            // }
            // if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
            //     // TODO
            // }
            // ImGui::Separator();
            // ImGui::MenuItem("Import", "Ctrl+I", &this->_importer.state);
            // ImGui::Separator();
            // ImGui::MenuItem("Export", "Ctrl+Shift+E", &this->_exporter.state);
            ImGui::MenuItem("Settings", "Ctrl+P", &this->_settings.state);
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Ctrl+Q"))
                glfwSetWindowShouldClose(this->_window.get(), true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            // TODO: Fix the console state
            // console = shared_ptr<Console>();
            ImGui::MenuItem("Console", "Ctrl+O", &this->_console->state);
            ImGui::MenuItem("Weather", "Ctrl+W", &this->_weather.state);
            ImGui::MenuItem("Camera Manager", "Ctrl+M", &this->_cameraManager.state);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}