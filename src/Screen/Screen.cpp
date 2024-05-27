#include "Screen.hpp"

using namespace Explorer400D;

extern bool stopSignal;

Screen::Screen()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    if (!(this->_window = glfwCreateWindow(1600, 900, "Explorer400D", NULL, NULL))) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(this->_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Initialize ImGui with GLFW & OpenGL && ImPlot
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Initialize Settings
    // this->_imguiIniPath = this->_settings->getSettingsPath() + "/imgui.ini";

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.IniFilename = this->_imguiIniPath.c_str();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Add the modules
    this->_modules.push_back(&this->_settings);
    this->_modules.push_back(&this->_console);
    this->_modules.push_back(&this->_cameraManager);

    // Load the settings

    // Start the screen loop
    this->_screenRender();
}

Screen::~Screen()
{
    // Save the settings

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->_window);
    glfwTerminate();
}

void Screen::_screenRender()
{
    while (true) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        if (this->_state == ApplicationState::STOPPED)
            break;

        if (stopSignal || glfwWindowShouldClose(this->_window))
            this->_state = ApplicationState::STOPPING;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Dockspace
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Main window
        switch (this->_state) {
        case ApplicationState::LOADING:
            this->_screenInit();
            break;
        case ApplicationState::RUNNING:
            this->_screenLoop();
            break;
        case ApplicationState::STOPPING:
            this->_screenClose();
            break;
        }

        // Render the window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the buffers
        glfwSwapBuffers(this->_window);
        glfwPollEvents();
    }
}

void Screen::_screenInit()
{
    spdlog::info("Initializing modules..");

    for (auto &module : this->_modules)
        module->moduleInit();

    spdlog::info("Loading settings..");

    for (auto &module : this->_modules)
        module->moduleLoadSettings();

    spdlog::info("Modules initialized");
    this->_state = ApplicationState::RUNNING;
}

void Screen::_screenLoop()
{
    for (auto &module : this->_modules)
        if (module->state)
            module->moduleLoop();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Ctrl+Q"))
                glfwSetWindowShouldClose(this->_window, true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Camera Manager", "Ctrl+M", &this->_cameraManager.state);
            // ImGui::MenuItem("ImgViewer", "Ctrl+I", &this->_imgViewer->state);
            // ImGui::MenuItem("Weather", "Ctrl+W", &this->_weather->state);
            // ImGui::MenuItem("Moon", "Ctrl+K", &this->_moon->state);
            // ImGui::MenuItem("Map", "Ctrl+Alt+M", &this->_map->state);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            // get Settings
            ImGui::MenuItem("Preferences", "Ctrl+P", &this->_settings.state);
            // ImGui::MenuItem("Reset Widgets", "Ctrl+R", &this->_resetWidgets);
            // ImGui::Separator();
            // ImGui::MenuItem("Save Config", "Ctrl+Shift+S", &);
            // TODO Themes choices
            ImGui::Separator();
            ImGui::MenuItem("Console", "Ctrl+O", &this->_console.state);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Screen::_screenClose()
{

    spdlog::info("Saving settings..");

    for (auto &module : this->_modules)
        module->moduleSaveSettings();

    spdlog::info("Closing modules..");

    for (auto &module : this->_modules)
        module->moduleClose();

    spdlog::info("Modules closed");

    this->_state = ApplicationState::STOPPED;
}