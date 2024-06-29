#include "CameraManager.hpp"

using namespace Explorer400D;

CameraManager::CameraManager(Settings &settings) : _settings(&settings)
{
    this->_context = nullptr;
    this->_cameraList = nullptr;
    this->_camera = nullptr;
    this->_portInfoList = nullptr;

    this->_numCameras = 0;
    this->_isCameraSelected = false;
    this->_selectedCameraName = "Select a Camera";
    this->_selectedCameraPort = "";
    this->_cameraSummary = "";

    this->_folderPath = "";
    this->_folderDialog = false;
    this->_initFolderPath = false;

    this->_lastCapture = 0;
    this->_inputShots = 0;
    this->_inputTime = 0;
    this->_saveInputTime = 0;

    this->_cameraConfigPath = "";
    this->_configDialog = false;
    this->_initConfigPath = false;
}

void CameraManager::_listConnectedCameras()
{
    if (this->_cameraList)
        gp_list_free(this->_cameraList);
    gp_list_new(&this->_cameraList);

    gp_camera_autodetect(this->_cameraList, this->_context);

    this->_numCameras = gp_list_count(this->_cameraList);

    std::string info = "Found " + std::to_string(this->_numCameras) + " camera(s)";
    spdlog::info(info);
    this->_selectedCameraName = info;
}

void CameraManager::_connectCamera()
{
    if (this->_numCameras == 1 && this->_isCameraSelected == false) {
        const char *cameraName;
        const char *cameraPort;

        gp_list_get_name(this->_cameraList, 0, &cameraName);
        gp_list_get_value(this->_cameraList, 0, &cameraPort);

        this->_selectedCameraName = cameraName;
        this->_selectedCameraPort = cameraPort;
        this->_isCameraSelected = true;
    }

    if (!this->_isCameraSelected) {
        spdlog::warn("No camera selected");
        return;
    }

    if (this->_camera)
        gp_camera_free(this->_camera);
    gp_camera_new(&this->_camera);

    if (this->_portInfoList)
        gp_port_info_list_free(this->_portInfoList);
    gp_port_info_list_new(&this->_portInfoList);
    gp_port_info_list_load(this->_portInfoList);

    GPPortInfo portInfo;
    int portNumber = gp_port_info_list_lookup_path(this->_portInfoList, this->_selectedCameraPort.c_str());
    gp_port_info_list_get_info(this->_portInfoList, portNumber, &portInfo);
    gp_camera_set_port_info(this->_camera, portInfo);

    if (gp_camera_init(this->_camera, this->_context) != GP_OK) {
        spdlog::error("Failed to initialize camera on port {}", this->_selectedCameraPort);
        gp_camera_free(this->_camera);
        return;
    }

    spdlog::info("Connected to {} on port {}", this->_selectedCameraName, portNumber);
}

void CameraManager::_disconnectCamera()
{
    if (this->_camera) {
        gp_camera_exit(this->_camera, this->_context);
        gp_camera_free(this->_camera);

        this->_camera = nullptr;
        this->_cameraSummary.clear();
        this->_isCameraSelected = false;

        spdlog::info("Disconnected from {}", this->_selectedCameraName);

        this->_selectedCameraName = "Select a Camera";
        this->_selectedCameraPort = "";

        if (this->_cameraList)
            gp_list_free(this->_cameraList);
        if (this->_portInfoList)
            gp_port_info_list_free(this->_portInfoList);

        this->_cameraList = nullptr;
        this->_portInfoList = nullptr;
    }
}

void CameraManager::_takePicture()
{
    CameraFilePath cameraFilePath;
    gp_camera_capture(this->_camera, GP_CAPTURE_IMAGE, &cameraFilePath, this->_context);

    CameraFile *cameraFile;
    gp_file_new(&cameraFile);
    gp_camera_file_get(this->_camera, cameraFilePath.folder, cameraFilePath.name, GP_FILE_TYPE_NORMAL, cameraFile, this->_context);

    std::string filePath = this->_folderPath + _PLAT_SEPARATOR + cameraFilePath.name;
    gp_file_save(cameraFile, filePath.c_str());
    this->_lastPicturePath = filePath;
    this->_newPicture.store(true);

    gp_file_free(cameraFile);
    spdlog::info("Saved picture to {}", filePath);

    gp_camera_file_delete(this->_camera, cameraFilePath.folder, cameraFilePath.name, this->_context);
}

void CameraManager::_configCameraLoop(CameraWidget *widget, std::string path)
{
    const char *label = nullptr, *name = nullptr;
    std::string useLabel = "";
    int childCount = 0;

    gp_widget_get_label(widget, &label);
    gp_widget_get_name(widget, &name);

    if (strlen(name))
        useLabel = name;
    else
        useLabel = label;

    path += "/" + useLabel;
    this->_cameraSettings.push_back(CameraSettings(widget, path));

    childCount = gp_widget_count_children(widget);

    for (size_t idx = 0; idx < childCount; idx++) {
        CameraWidget *childWidget = nullptr;

        if (gp_widget_get_child(widget, idx, &childWidget) != GP_OK) {
            spdlog::warn("Failed to get child widget at index {}", idx);
            continue;
        }

        this->_configCameraLoop(childWidget, path);
    }
}

void CameraManager::_configGetCamera()
{
    this->_cameraSettings.clear();

    gp_camera_get_config(this->_camera, &this->_configWidget, this->_context);

    this->_configCameraLoop(this->_configWidget, "");

    this->_configCameraLoaded.store(true);
}

void CameraManager::_configCameraSetup()
{
    static char inputPath[_INPUT_TEXT_SIZE] = ".";

    ImGui::InputText("Camera Config File", inputPath, sizeof(inputPath), ImGuiInputTextFlags_ReadOnly);

    if (this->_initConfigPath) {
        std::memset(inputPath, 0, sizeof(inputPath));
        std::string path = this->_cameraConfigPath + _PLAT_SEPARATOR + this->_cameraConfigFile;
        std::strcat(inputPath, path.c_str());
        // load camera config
        this->_initConfigPath = false;
    }

    if (ImGui::Button("Open Config File")) {
        this->_configDialog = true;
        this->_uiDisable = true;
    }

    if (this->_configDialog) {
        ImGui::EndDisabled();

        IGFD::FileDialogConfig config;

        config.path = this->_cameraConfigPath;
        config.fileName = this->_cameraConfigFile;
        config.flags = ImGuiFileDialogFlags_DontShowHiddenFiles;

        ImGuiFileDialog::Instance()->OpenDialog("ChooseCameraConfig", "Choose a Camera Config File", ".json", config);

        if (ImGuiFileDialog::Instance()->Display("ChooseCameraConfig")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                this->_cameraConfigPath = ImGuiFileDialog::Instance()->GetCurrentPath();
                this->_cameraConfigFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
                this->_initConfigPath = true;
                this->_configDialog = false;
                this->_uiDisable = false;
            }
            ImGuiFileDialog::Instance()->Close();
            this->_configDialog = false;
            this->_uiDisable = false;
        }

        ImGui::BeginDisabled(this->_uiDisable);
    }

    if (this->_cameraConfigPath.empty() || this->_cameraConfigFile.empty())
        return;
    // TODO Check config file

    if (ImGui::Button("Get Camera Config")) {
        this->_configCameraLoaded.store(false);
        this->_configCameraBinded = false;
        this->moduleThreadInit(std::bind(&CameraManager::_configGetCamera, this));
    }

    if (!this->_configCameraLoaded.load())
        return;

    if (!this->_configCameraBinded) {
        for (auto &cameraSetting : this->_cameraSettings) {
            cameraSetting.initBinding();
            cameraSetting.initWidget();
        }
        this->_configCameraBinded = true;
        spdlog::info("Camera config loaded");
    }

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Camera Config", nodeFlags)) {
        for (auto &cameraSetting : this->_cameraSettings)
            cameraSetting.drawWidget();
        ImGui::TreePop();
    }
}

void CameraManager::_cameraStartupSetup()
{
    // ! Without a label the combo box doesn't work
    if (ImGui::BeginCombo("Camera List", this->_selectedCameraName.c_str())) {
        if (this->_cameraList != nullptr) {
            for (size_t idx = 0; idx < this->_numCameras; idx++) {
                const char *cameraName;
                const char *cameraPort;

                gp_list_get_name(this->_cameraList, idx, &cameraName);
                gp_list_get_value(this->_cameraList, idx, &cameraPort);

                bool isSelected = (this->_selectedCameraName == cameraName);

                if (ImGui::Selectable(cameraName, isSelected)) {
                    this->_selectedCameraName = cameraName;
                    this->_selectedCameraPort = cameraPort;
                    this->_isCameraSelected = true;
                }

                if (this->_selectedCameraName == cameraName)
                    ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Auto Detect"))
        this->moduleThreadInit(std::bind(&CameraManager::_listConnectedCameras, this));

    ImGui::SameLine();
    if (ImGui::Button("Connect"))
        this->moduleThreadInit(std::bind(&CameraManager::_connectCamera, this));

    if (this->_camera == nullptr)
        return;

    ImGui::SameLine();
    if (ImGui::Button("Disconnect"))
        this->moduleThreadInit(std::bind(&CameraManager::_disconnectCamera, this));

    ImGui::SameLine();
    if (ImGui::Button("Show Camera Info"))
        ImGui::OpenPopup("Camera Info");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Camera Info", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (this->_cameraSummary.empty()) {
            CameraText text;
            gp_camera_get_summary(this->_camera, &text, this->_context);
            this->_cameraSummary = text.text;
        }

        ImGui::Text("%s", this->_cameraSummary.c_str());

        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    this->_cameraFolderDestination();
}

void CameraManager::_cameraFolderDestination()
{
    static char inputPath[_INPUT_TEXT_SIZE] = ".";

    ImGui::InputText("Pictures Destination", inputPath, sizeof(inputPath), ImGuiInputTextFlags_ReadOnly);

    if (this->_initFolderPath) {
        std::memset(inputPath, 0, sizeof(inputPath));
        std::strcat(inputPath, this->_folderPath.c_str());
        this->_initFolderPath = false;
    }

    if (ImGui::Button("Choose Folder")) {
        this->_folderDialog = true;
        this->_uiDisable = true;
    }

    if (this->_folderDialog) {
        ImGui::EndDisabled();

        IGFD::FileDialogConfig config;

        config.path = inputPath;
        config.flags = ImGuiFileDialogFlags_DontShowHiddenFiles;

        ImGuiFileDialog::Instance()->OpenDialog("ChooseFolder", "Choose a Directory", nullptr, config);

        if (ImGuiFileDialog::Instance()->Display("ChooseFolder")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string folderPathName = ImGuiFileDialog::Instance()->GetCurrentPath();
                this->_folderPath = folderPathName;
                this->_initFolderPath = true;
                this->_folderDialog = false;
                this->_uiDisable = false;
            }
            ImGuiFileDialog::Instance()->Close();
            this->_folderDialog = false;
            this->_uiDisable = false;
        }

        ImGui::BeginDisabled(this->_uiDisable);
    }

    if (this->_folderPath.empty())
        return;

    this->_cameraPictureTrigger();
}

void CameraManager::_cameraPictureTrigger()
{
    static bool autoCapture = false;
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (autoCapture) {
        ImGui::BeginDisabled();
        ImGui::Button("Take Picture");
        ImGui::EndDisabled();
    } else {
        if (ImGui::Button("Take Picture"))
            this->moduleThreadInit(std::bind(&CameraManager::_takePicture, this));
    }

    ImGui::SameLine();
    ImGui::Checkbox("Auto Capture", &autoCapture);

    static char inputTime[32] = "15", inputShots[32] = "5";
    static bool captureSession = false;

    // ! to repair
    if (autoCapture) {
        if (ImGui::TreeNodeEx("Auto Capture", nodeFlags)) {
            ImGui::InputText("Capture Frequency", inputTime, sizeof(inputTime), ImGuiInputTextFlags_CharsDecimal);
            ImGui::InputText("Shots Number", inputShots, sizeof(inputTime), ImGuiInputTextFlags_CharsDecimal);
            if (!captureSession) {
                if (ImGui::Button("Start Capture Session")) {
                    try {
                        this->_inputShots = std::stoi(inputShots);
                        this->_inputTime = 5; // Countdown
                        this->_saveInputTime = std::stoi(inputTime);
                        this->_lastCapture = std::time(0);
                        captureSession = true;
                    } catch (std::exception &error) {
                        spdlog::warn(error.what());
                        captureSession = false;
                    }
                }
            }

            if (captureSession) {
                if (std::time(0) - this->_lastCapture >= this->_inputTime) {
                    this->_lastCapture = std::time(0);
                    this->_inputShots--;
                    try {
                        this->moduleThreadInit(std::bind(&CameraManager::_takePicture, this));
                        this->_inputTime = this->_saveInputTime;
                    } catch (std::exception &error) {
                        spdlog::error(error.what());
                    }
                }
                if (this->_inputShots <= 0)
                    captureSession = false;
                ImGui::Text("Time left: %ld", this->_inputTime - (std::time(0) - this->_lastCapture));
                ImGui::SameLine();
                ImGui::Text("Shots left: %d", this->_inputShots);
                ImGui::SameLine();
                if (ImGui::Button("Stop Capture Session"))
                    captureSession = false;
            }
            ImGui::TreePop();
        }
    }

    if (this->_newPicture.load()) {
        this->_imgViewer.release();
        this->_imgViewer = std::make_unique<ImgViewer>(this->_lastPicturePath);
        this->_imgViewer->loadImg();
        this->_newPicture.store(false);
    } else {
        if (ImGui::TreeNodeEx("Picture Visualizer", nodeFlags)) {
            if (this->_imgViewer != nullptr)
                this->_imgViewer->moduleLoop();
            ImGui::TreePop();
        }
    }
}

void CameraManager::moduleInit()
{
    this->_context = gp_context_new();

    gp_context_set_error_func(
        this->_context, [](GPContext *context, const char *text, void *data) { spdlog::error("GPhoto2 error: {}", text); }, nullptr);
    gp_context_set_message_func(
        this->_context, [](GPContext *context, const char *text, void *data) { spdlog::info("GPhoto2 message: {}", text); }, nullptr);
    gp_context_set_status_func(
        this->_context, [](GPContext *context, const char *text, void *data) { spdlog::info("GPhoto2 status: {}", text); }, nullptr);
}

void CameraManager::moduleLoop()
{
    ImGui::Begin("Camera Manager", &this->state);

    ImGui::BeginDisabled(this->_uiDisable);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Set Camera", nodeFlags)) {
        this->_cameraStartupSetup();
        ImGui::TreePop();
    }

    if (this->_camera != nullptr) {
        if (ImGui::TreeNodeEx("Camera Settings", nodeFlags)) {
            this->_configCameraSetup();
            ImGui::TreePop();
        }
    }

    ImGui::EndDisabled();

    ImGui::End();
}

void CameraManager::moduleClose()
{
    if (this->_imgViewer)
        this->_imgViewer.release();
    if (this->_configWidget)
        gp_widget_free(this->_configWidget);
    if (this->_camera) {
        gp_camera_exit(this->_camera, this->_context);
        gp_camera_free(this->_camera);
    }
    if (this->_cameraList)
        gp_list_free(this->_cameraList);
    if (this->_portInfoList)
        gp_port_info_list_free(this->_portInfoList);
    gp_context_unref(this->_context);
}

void CameraManager::moduleSettingsLoad()
{
    json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::CameraManager::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
    (obj = this->_settings->getSetting("Explorer400D::CameraManager::FolderPath")) != nullptr ? this->_folderPath = obj.get<std::string>() : this->_folderPath = "";

    std::string cameraConfigPath = ".";
    (obj = this->_settings->getSetting("Explorer400D::CameraManager::ConfigPath")) != nullptr ? cameraConfigPath = obj.get<std::string>() : cameraConfigPath = "";

    this->_initFolderPath = this->_settings->fileExists(this->_folderPath, "Folder");
    this->_initConfigPath = this->_settings->fileExists(cameraConfigPath, "Config file");

    if (this->_initConfigPath) {
        this->_cameraConfigPath = cameraConfigPath.substr(0, cameraConfigPath.find_last_of(_PLAT_SEPARATOR));
        this->_cameraConfigFile = cameraConfigPath.substr(cameraConfigPath.find_last_of(_PLAT_SEPARATOR) + 1);
    }
}

void CameraManager::moduleSettingsSave()
{
    this->_settings->setSetting("Explorer400D::CameraManager::State", this->state);
    this->_settings->setSetting("Explorer400D::CameraManager::FolderPath", this->_folderPath);

    std::string cameraConfigPath = this->_cameraConfigPath + _PLAT_SEPARATOR + this->_cameraConfigFile;
    this->_settings->setSetting("Explorer400D::CameraManager::ConfigPath", cameraConfigPath);
}