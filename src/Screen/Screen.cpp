#include "Screen.hpp"

using namespace Explorer400D;

extern bool stopSignal;

// #ifndef STB_IMAGE_IMPLEMENTATION
//     #define STB_IMAGE_IMPLEMENTATION
//     #include "stb_image.h"

Screen::Screen() : _console(this->_settings), _cameraManager(this->_settings), _cameraOMatic(this->_settings), _map(this->_settings, this->_webFetch), _weather(this->_settings, this->_webFetch), _selfGuiding(this->_settings, this->_webFetch)
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
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    this->_settings.setupStyle();

    ImGui_ImplGlfw_InitForOpenGL(this->_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Add the modules
    this->_modules.push_back(&this->_settings);
    this->_modules.push_back(&this->_webFetch);
    this->_modules.push_back(&this->_console);
    this->_modules.push_back(&this->_cameraManager);
    this->_modules.push_back(&this->_cameraOMatic);
    this->_modules.push_back(&this->_map);
    this->_modules.push_back(&this->_weather);
    this->_modules.push_back(&this->_selfGuiding);

    // Set Window Icon
    // this->_setWindowIcon();

    // Start the screen loop
    this->_screenRender();
}

Screen::~Screen()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->_window);
    glfwTerminate();
}

// void Screen::_setWindowIcon()
// {
//     if (!std::filesystem::exists("icon.png")) {
//         spdlog::warn("icon.png not found");
//         return;
//     }

//     GLFWimage images[1];
//     images[0].pixels = stbi_load("icon.png", &images[0].width, &images[0].height, 0, 4);

//     if (!images[0].pixels) {
//         spdlog::warn("Failed to load icon.png");
//         return;
//     }

//     glfwSetWindowIcon(this->_window, 1, images);
// }

void Screen::_screenRender()
{
    glfwSwapInterval(4);

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
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

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

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // Swap the buffers
        glfwSwapBuffers(this->_window);
        glfwPollEvents();
    }
}

void Screen::_screenInit()
{
    if (!this->_threadRunning.load()) {
        this->_moduleThread = std::thread([this]() {
            this->_threadRunning.store(true);
            spdlog::info("Initializing modules..");

            this->_threadState.store(InitThreadState::INIT);
            for (this->_moduleIndex.store(0); this->_moduleIndex.load() < this->_modules.size(); this->_moduleIndex++)
                this->_modules[this->_moduleIndex]->moduleInit();

            spdlog::info("Loading settings..");

            this->_threadState.store(InitThreadState::SAVE);
            for (this->_moduleIndex.store(0); this->_moduleIndex.load() < this->_modules.size(); this->_moduleIndex++)
                this->_modules[this->_moduleIndex]->moduleSettingsLoad();

            spdlog::info("Modules initialized");

            this->_stopSignal.store(true);
        });
    }

    if (this->_stopSignal.load()) {
        this->_moduleThread.join();
        this->_threadRunning.store(false);
        this->_stopSignal.store(false);
        this->_state = ApplicationState::RUNNING;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Loading Window", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings);

    switch (this->_threadState.load()) {
    case InitThreadState::INIT:
        ImGui::Text("Initializing modules..");
        ImGui::ProgressBar((float)this->_moduleIndex.load() / this->_modules.size());
        break;
    case InitThreadState::SAVE:
        ImGui::Text("Loading settings..");
        ImGui::ProgressBar((float)this->_moduleIndex.load() / this->_modules.size());
        break;
    }

    ImGui::End();
}

void Screen::_screenLoop()
{
    for (auto &module : this->_modules) {
        if (module->state) {
            module->moduleLoop();
            module->moduleThreadLoop();
        }
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Ctrl+Q"))
                glfwSetWindowShouldClose(this->_window, true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Camera Manager", "Ctrl+M", &this->_cameraManager.state);
            ImGui::BeginDisabled();
            ImGui::MenuItem("CameraOMatic", "Ctrl+O", &this->_cameraOMatic.state);
            ImGui::EndDisabled();
            ImGui::MenuItem("Map", "Ctrl+Alt+M", &this->_map.state);
            ImGui::MenuItem("Weather", "Ctrl+W", &this->_weather.state);
            ImGui::MenuItem("SelfGuiding", "Ctrl+Shift+S", &this->_selfGuiding.state);
            // ImGui::MenuItem("ImgViewer", "Ctrl+I", &this->_imgViewer->state);
            // ImGui::MenuItem("Moon", "Ctrl+K", &this->_moon->state);
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
    if (!this->_threadRunning.load()) {
        this->_moduleThread = std::thread([this]() {
            this->_threadRunning.store(true);
            spdlog::info("Saving settings..");

            this->_threadState.store(InitThreadState::SAVE);
            for (this->_moduleIndex.store(0); this->_moduleIndex.load() < this->_modules.size(); this->_moduleIndex++)
                this->_modules[this->_moduleIndex]->moduleSettingsSave();

            spdlog::info("Closing modules..");

            this->_threadState.store(InitThreadState::INIT);
            for (this->_moduleIndex.store(0); this->_moduleIndex.load() < this->_modules.size(); this->_moduleIndex++)
                this->_modules[this->_moduleIndex]->moduleClose();

            spdlog::info("Modules closed");

            this->_stopSignal.store(true);
        });
    }

    if (this->_stopSignal.load()) {
        this->_moduleThread.join();
        this->_threadRunning.store(false);
        this->_stopSignal.store(false);
        this->_state = ApplicationState::STOPPED;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Closing Window", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings);

    switch (this->_threadState.load()) {
    case InitThreadState::INIT:
        ImGui::Text("Closing modules..");
        ImGui::ProgressBar((float)this->_moduleIndex.load() / this->_modules.size());
        break;
    case InitThreadState::SAVE:
        ImGui::Text("Saving settings..");
        ImGui::ProgressBar((float)this->_moduleIndex.load() / this->_modules.size());
        break;
    }

    ImGui::End();
}