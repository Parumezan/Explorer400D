#include "CameraManager.hpp"

using namespace Explorer400D;

CameraManager::CameraManager(Settings &settings)
{
    this->_settings = &settings;

    this->_context = nullptr;
    this->_cameraList = nullptr;
    this->_camera = nullptr;
    this->_portInfoList = nullptr;

    this->_isCameraSelected = false;
    this->_selectedCameraName = "Select a Camera";
    this->_selectedCameraPort = "";
    this->_cameraSummary = "";

    this->_folderPath = "";

    this->_lastCapture = 0;
    this->_inputShots = 0;
    this->_inputTime = 0;
}

void CameraManager::listConnectedCameras()
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

void CameraManager::connectCamera()
{
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

void CameraManager::disconnectCamera()
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

void CameraManager::takePicture()
{
    CameraFilePath cameraFilePath;
    gp_camera_capture(this->_camera, GP_CAPTURE_IMAGE, &cameraFilePath, this->_context);

    CameraFile *cameraFile;
    gp_file_new(&cameraFile);
    gp_camera_file_get(this->_camera, cameraFilePath.folder, cameraFilePath.name, GP_FILE_TYPE_NORMAL, cameraFile, this->_context);

    std::string filePath = this->_folderPath + "/" + cameraFilePath.name;
    gp_file_save(cameraFile, filePath.c_str());

    gp_file_free(cameraFile);
    spdlog::info("Saved picture to {}", filePath);

    gp_camera_file_delete(this->_camera, cameraFilePath.folder, cameraFilePath.name, this->_context);
}

void CameraManager::setupCamera()
{
    // ! Without a label the combo box doesn't work
    if (ImGui::BeginCombo("-", this->_selectedCameraName.c_str())) {
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

    ImGui::SameLine();
    if (ImGui::Button("Auto Detect"))
        this->moduleThreadInit(std::bind(&CameraManager::listConnectedCameras, this));

    ImGui::SameLine();
    if (ImGui::Button("Connect"))
        this->moduleThreadInit(std::bind(&CameraManager::connectCamera, this));

    if (this->_camera == nullptr)
        return;

    ImGui::SameLine();
    if (ImGui::Button("Disconnect"))
        this->moduleThreadInit(std::bind(&CameraManager::disconnectCamera, this));

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

    this->folderPart();
}

void CameraManager::folderPart()
{
    static char inputPath[64] = ".";

    ImGui::InputText("", inputPath, sizeof(inputPath));

    if (this->_initFolderPath) {
        std::memset(inputPath, 0, sizeof(inputPath));
        std::strcat(inputPath, this->_folderPath.c_str());
        this->_initFolderPath = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Open folder")) {
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

    this->picturePart();
}

void CameraManager::picturePart()
{
    if (ImGui::Button("Take Picture"))
        this->moduleThreadInit(std::bind(&CameraManager::takePicture, this));

    static bool autoCapture = false;
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::SameLine();
    ImGui::Checkbox("Auto Capture", &autoCapture);

    static char inputTime[32] = "15", inputShots[32] = "5";
    static bool captureSession = false;

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
                        this->moduleThreadInit(std::bind(&CameraManager::takePicture, this));
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
        this->setupCamera();
        ImGui::TreePop();
    }

    ImGui::EndDisabled();

    ImGui::End();
}

void CameraManager::moduleClose()
{
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
    nlohmann::json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::CameraManager::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
    (obj = this->_settings->getSetting("Explorer400D::CameraManager::FolderPath")) != nullptr ? this->_folderPath = obj.get<std::string>() : this->_folderPath = "";

    if (!this->_folderPath.empty() && !std::filesystem::exists(this->_folderPath)) {
        spdlog::warn("Folder path {} doesn't exist", this->_folderPath);
        this->_folderPath.clear();
    } else {
        this->_initFolderPath = true;
    }
}

void CameraManager::moduleSettingsSave()
{
    this->_settings->setSetting("Explorer400D::CameraManager::State", this->state);

    if (!this->_folderPath.empty())
        this->_settings->setSetting("Explorer400D::CameraManager::FolderPath", this->_folderPath);
}