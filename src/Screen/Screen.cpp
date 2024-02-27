#include "Screen.hpp"

using namespace Explorer400D;

Screen::Screen(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings, std::shared_ptr<ImgViewer> imgViewer, std::shared_ptr<CameraManager> cameraManager, std::shared_ptr<Weather> weather, std::shared_ptr<Moon> moon)
    : _console(console), _settings(settings), _imgViewer(imgViewer), _cameraManager(cameraManager), _weather(weather), _moon(moon)
{
    this->_console->info("Initializing Screen module...");
    try {
        // Initialize the window with GLFW
        if (!glfwInit())
            return;
        this->_window = std::shared_ptr<GLFWwindow>(glfwCreateWindow(1600, 900, "Explorer400D", NULL, NULL), glfwDestroyWindow);
        if (!this->_window)
            return;
        glfwMakeContextCurrent(this->_window.get());

        // Initialize ImGui with GLFW & OpenGL && ImPlot
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();

        // Initialize Settings
        this->_imguiIniPath = this->_settings->getSettingsPath() + "/imgui.ini";

        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.IniFilename = this->_imguiIniPath.c_str();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(this->_window.get(), true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // Initialize widgets
        this->_resetWidgets = false;
        this->_widgetStatus = this->_settings->getWidgetsStatus();
        this->_console->state = this->_widgetStatus["Console"];
        this->_settings->state = this->_widgetStatus["Settings"];
        this->_cameraManager->state = this->_widgetStatus["CameraManager"];
        this->_imgViewer->state = this->_widgetStatus["ImgViewer"];
        this->_weather->state = this->_widgetStatus["Weather"];
        this->_moon->state = this->_widgetStatus["Moon"];

        // Initialize things after initializing GUI if needed (like images)
        // this->_moon.initFrame();
        // this->_cameraManager.initFrame();
        this->_imgViewer->initFrame();

        // Load the settings
        this->loadConfig();
    } catch (const std::exception &error) {
        this->_console->throwException(error.what());
    }
    this->_console->info("Screen module initialized");
    this->windowLoop();
}

Screen::~Screen()
{
    // Save the settings
    this->_console->info("Closing save settings...");
    this->saveWidgetsStatus();
    this->_imgViewer->saveSettings();
    this->_cameraManager->saveSettings();
    this->_settings->saveSettings(this->_widgetStatus);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->_window.get());
    glfwTerminate();
}

void Screen::loadConfig()
{
    ImPlot::GetStyle().UseLocalTime = this->_settings->getSettingsVar("IMPLOT_UseLocalTime") == "true" ? true : false;
    ImPlot::GetStyle().UseISO8601 = this->_settings->getSettingsVar("IMPLOT_UseISO8601") == "true" ? true : false;
    ImPlot::GetStyle().Use24HourClock = this->_settings->getSettingsVar("IMPLOT_Use24HourClock") == "true" ? true : false;
}

void Screen::windowLoop()
{
    this->_console->info("Starting window loop...");
    while (!glfwWindowShouldClose(this->_window.get())) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        if (this->_console->stopSequence)
            break;

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

void Screen::saveWidgetsStatus()
{
    this->_widgetStatus["Console"] = this->_console->state;
    this->_widgetStatus["Settings"] = this->_settings->state;
    this->_widgetStatus["CameraManager"] = this->_cameraManager->state;
    this->_widgetStatus["ImgViewer"] = this->_imgViewer->state;
    this->_widgetStatus["Weather"] = this->_weather->state;
    this->_widgetStatus["Moon"] = this->_moon->state;
}

void Screen::resetWidgetsStatus()
{
    if (this->_resetWidgets) {
        this->_console->state = false;
        this->_settings->state = false;
        this->_cameraManager->state = false;
        this->_weather->state = false;
        this->_moon->state = false;

        // update the reset status
        this->_resetWidgets = false;
    }
}

void Screen::mainMenuBar()
{
    if (this->_console->state)
        this->_console->frameLoop();
    if (this->_cameraManager->state)
        this->_cameraManager->frameLoop();
    if (this->_imgViewer->state)
        this->_imgViewer->frameLoop();
    if (this->_weather->state)
        this->_weather->frameLoop();
    if (this->_moon->state)
        this->_moon->frameLoop();
    if (this->_settings->state)
        this->_settings->frameLoop();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Ctrl+Q"))
                glfwSetWindowShouldClose(this->_window.get(), true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Camera Manager", "Ctrl+M", &this->_cameraManager->state);
            ImGui::MenuItem("ImgViewer", "Ctrl+I", &this->_imgViewer->state);
            ImGui::MenuItem("Weather", "Ctrl+W", &this->_weather->state);
            ImGui::MenuItem("Moon", "Ctrl+K", &this->_moon->state);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            ImGui::MenuItem("Preferences", "Ctrl+P", &this->_settings->state);
            ImGui::MenuItem("Reset Widgets", "Ctrl+R", &this->_resetWidgets);
            // ImGui::Separator();
            // ImGui::MenuItem("Save Config", "Ctrl+Shift+S", &);
            // TODO Themes choices
            ImGui::Separator();
            ImGui::MenuItem("Console", "Ctrl+O", &this->_console->state);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    this->resetWidgetsStatus();
}