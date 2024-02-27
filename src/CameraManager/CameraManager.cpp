#include "CameraManager.hpp"

using namespace Explorer400D;

CameraManager::CameraManager(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings, std::shared_ptr<ImgViewer> imgViewer) : _console(console), _settings(settings), _imgViewer(imgViewer)
{
    this->_console->info("Initializing CameraManager module...");
    this->_selectedCameraName = "Select a Camera";
    this->_initFolderPath = false;
    this->_showCameraInfo = false;
    this->_folderDialog = false;
    this->_lastCapture = 0;
    this->_inputShots = 0;
    this->_inputTime = 0;
    try {
        this->initFolder();
    } catch (std::exception &error) {
        this->_console->throwException(error.what());
    }
    this->_console->info("CameraManager module started");
}

CameraManager::~CameraManager()
{
}

void CameraManager::autoDetectCameras()
{
    gphoto2pp::GPPortInfoListWrapper portInfoListWrapper;
    gphoto2pp::CameraAbilitiesListWrapper cameraAbilitiesListWrapper;
    gphoto2pp::CameraListWrapper cameraList = cameraAbilitiesListWrapper.listDetect(portInfoListWrapper);

    if (portInfoListWrapper.count() == 0)
        throw std::runtime_error("No camera detected");
    this->_console->info(std::to_string(portInfoListWrapper.count()) + " port(s) detected");
    this->_cameraList = std::make_shared<gphoto2pp::CameraListWrapper>(cameraList);
    this->_console->info(std::to_string(this->_cameraList->count()) + " camera(s) detected");
}

void CameraManager::connectCamera()
{
    this->_camera = std::make_shared<gphoto2pp::CameraWrapper>(this->_model, this->_port);
    this->_console->info("Camera connected: " + this->_model + " on port " + this->_port);
    this->_cameraSummary = this->_camera->getSummary();
}

void CameraManager::takePicture()
{
    gphoto2pp::CameraFilePathWrapper cameraFilePath;
    gphoto2pp::CameraFileWrapper cameraFile;

    this->_console->info("Taking picture...");
    cameraFilePath = this->_camera->capture(gphoto2pp::CameraCaptureTypeWrapper::Image);

    this->_console->info("Getting picture...");
    cameraFile = this->_camera->fileGet(cameraFilePath.Folder, cameraFilePath.Name, gphoto2pp::CameraFileTypeWrapper::Normal);

    this->_console->info("Saving picture...");
    std::string path = this->_folderPath + "/" + cameraFilePath.Name;
    cameraFile.save(path);

    if (this->_imgViewer->getFolderPath() != "false" && this->_imgViewer->getFolderPath() == this->_folderPath)
        this->_imgViewer->reloadImages();

    this->_console->info("Deleting picture from camera...");
    this->_camera->fileDelete(cameraFilePath.Folder, cameraFilePath.Name);

    this->_imgPreview = std::make_shared<ImgLoader>(this->_console, path);
}

void CameraManager::setCamera()
{
    // ! Without a label the combo box doesn't work
    if (ImGui::BeginCombo("-", this->_selectedCameraName.c_str())) {
        if (this->_cameraList != nullptr) {
            for (int idx = 0; idx < this->_cameraList->count(); idx++) {
                if (ImGui::Selectable(this->_cameraList->getName(idx).c_str())) {
                    this->_selectedCameraName = this->_cameraList->getName(idx);
                    std::pair<std::string, std::string> cameraInfo = this->_cameraList->getPair(idx);
                    this->_model = cameraInfo.first;
                    this->_port = cameraInfo.second;
                }
                if (this->_selectedCameraName == this->_cameraList->getName(idx))
                    ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Auto Detect")) {
        try {
            this->autoDetectCameras();
        } catch (std::exception &error) {
            this->_console->error(error.what());
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Connect")) {
        try {
            this->connectCamera();
        } catch (std::exception &error) {
            this->_console->error(error.what());
        }
    }

    if (this->_camera == nullptr)
        return;

    ImGui::SameLine();
    if (ImGui::Button("Disconnect")) {
        this->_camera = nullptr;
        this->_cameraList = nullptr;
        this->_selectedCameraName = "Select a Camera";
        this->_cameraSummary = "";
        return;
    }

    if (ImGui::Button("Show Camera Info"))
        ImGui::OpenPopup("Camera Info");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Camera Info", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", this->_cameraSummary.c_str());
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    this->folderPart();
}

void CameraManager::folderPart()
{
    static char inputPath[64] = "";

    ImGui::InputText("", inputPath, sizeof(inputPath));

    if (this->_initFolderPath) {
        std::memset(inputPath, 0, sizeof(inputPath));
        std::strcat(inputPath, this->_folderPath.c_str());
        this->_initFolderPath = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Open folder"))
        this->_folderDialog = true;

    if (this->_folderDialog) {
        ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_DontShowHiddenFiles;
        std::string path = this->_folderPath == "false" ? "." : this->_folderPath;
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFolder", "Choose a Directory", nullptr, path, 0, 0, flags);

        if (ImGuiFileDialog::Instance()->Display("ChooseFolder")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string folderPathName = ImGuiFileDialog::Instance()->GetFilePathName();
                this->_console->info("Selected folder: " + folderPathName);
                this->_folderPath = folderPathName;
                this->_initFolderPath = true;
                this->_folderDialog = false;
            }
            ImGuiFileDialog::Instance()->Close();
            this->_folderDialog = false;
        }
    }

    if (this->_folderPath == "false" || this->_folderPath == "")
        return;

    this->picturePart();
}

void CameraManager::picturePart()
{
    if (ImGui::Button("Take Picture")) {
        try {
            this->takePicture();
        } catch (std::exception &error) {
            this->_console->error(error.what());
        }
    }

    static bool autoCapture = false;
    static bool showImgViewer = false;
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGui::SameLine();
    ImGui::Checkbox("Auto Capture", &autoCapture);
    ImGui::SameLine();
    ImGui::Checkbox("Show Image Viewer", &showImgViewer);

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
                        this->_console->warn(error.what());
                        captureSession = false;
                    }
                }
            }

            if (captureSession) {
                if (std::time(0) - this->_lastCapture >= this->_inputTime) {
                    this->_lastCapture = std::time(0);
                    this->_inputShots--;
                    try {
                        this->takePicture();
                        this->_inputTime = this->_saveInputTime;
                    } catch (std::exception &error) {
                        this->_console->error(error.what());
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

    if (showImgViewer) {
        if (ImGui::TreeNodeEx("Image Viewer", nodeFlags)) {
            ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoGridLines;
            ImVec2 plotSize = ImGui::GetContentRegionAvail();

            if ((ImPlot::BeginPlot("Preview Image", plotSize, ImPlotFlags_CanvasOnly | ImPlotFlags_Equal)) && this->_imgPreview) {
                ImPlot::SetupAxes(0, 0, flags, flags);

                ImVec2 canvasSize = ImPlot::GetPlotPos();
                ImVec2 canvasMax = ImVec2(canvasSize.x + plotSize.x, canvasSize.y + plotSize.y);

                ImPlot::PushPlotClipRect();
                ImPlot::GetPlotDrawList()->AddRectFilled(canvasSize, canvasMax, IM_COL32_BLACK);
                ImPlot::PopPlotClipRect();

                ImPlot::PlotImage("Preview Image", this->_imgPreview->getImguiTexture(), ImVec2(0, 0), ImVec2(this->_imgPreview->getImgWidth(), this->_imgPreview->getImgHeight()));

                ImPlot::EndPlot();
            }
            ImGui::TreePop();
        }
    }
}

void CameraManager::initFolder()
{
    this->_folderPath = this->_settings->getSettingsVar("EX4D_CameraManager_FolderPath");

    if (this->_folderPath != "false") {
        this->_initFolderPath = true;
    }
}

void CameraManager::frameLoop()
{
    ImGui::Begin("Camera Manager", &this->state);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Set Camera", nodeFlags)) {
        this->setCamera();
        ImGui::TreePop();
    }

    ImGui::End();
}

void CameraManager::saveSettings()
{
    this->_settings->setSettingsVar("EX4D_CameraManager_FolderPath", this->_folderPath);
}