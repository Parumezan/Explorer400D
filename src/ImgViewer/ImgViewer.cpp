#include "ImgViewer.hpp"

using namespace Explorer400D;

ImgViewer::ImgViewer(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings) : _console(console), _settings(settings)
{
    this->_console->info("Initializing ImgViewer module...");
    this->_folderDialog = false;
    this->_initFolderPath = false;
    this->_loadImageFiles = false;
    this->_imageFiles = {};
    try {
        this->_mode = this->_settings->getSettingsVar("EX4D_ImgViewer_Mode");
        this->_lastMode = ImgViewerModes[0];
        if (this->_mode == "false" || this->_mode == "")
            this->_mode = ImgViewerModes[0];
        this->initFolder();
    } catch (std::exception &error) {
        this->_console->throwException(error.what());
    }
    this->_console->info("ImgViewer module started");
}

ImgViewer::~ImgViewer()
{
}

void ImgViewer::switchMode(std::string mode)
{
    for (size_t cpt = 0; cpt < ImgViewerModes.size(); cpt++) {
        if (ImgViewerModes[cpt] == mode) {
            this->_mode = ImgViewerModes[cpt];
            break;
        }
    }
}

void ImgViewer::initFolder()
{
    this->_folderPath = this->_settings->getSettingsVar("EX4D_ImgViewer_FolderPath");

    if (this->_folderPath != "false") {
        this->_initFolderPath = true;
        this->_loadImageFiles = true;
    }
}

void ImgViewer::folderMode()
{
    if (this->_mode != "Folder")
        return;

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    static char inputPath[64] = "";

    ImGui::InputText("", inputPath, sizeof(inputPath));

    // Init folder path condition
    if (this->_initFolderPath) {
        std::memset(inputPath, 0, sizeof(inputPath));
        std::strcat(inputPath, this->_folderPath.c_str());
        this->_initFolderPath = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Open folder"))
        this->_folderDialog = true;

    // Folder Choose Dialog
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
                this->_loadImageFiles = true;
                this->_folderDialog = false;
            }
            ImGuiFileDialog::Instance()->Close();
            this->_folderDialog = false;
        }
    }

    if (this->_folderPath != "false") {
        ImGui::SameLine();
        if (ImGui::Button("Refresh Images"))
            this->_loadImageFiles = true;
    }

    // Load image files
    if (this->_loadImageFiles) {
        this->_imageFiles.clear();
        for (const auto &entry : std::filesystem::directory_iterator(this->_folderPath)) {
            std::string extension = entry.path().extension().string();
            if (std::find(ImgTypeStb.begin(), ImgTypeStb.end(), extension) != ImgTypeStb.end() || std::find(ImgTypeRaw.begin(), ImgTypeRaw.end(), extension) != ImgTypeRaw.end())
                continue;
            this->_imageFiles.push_back({entry.path().filename().string(), entry.path().string(), extension, nullptr});
            // this->_imageFiles.push_back({entry.path().filename().string(), entry.path().string(), extension, std::make_shared<ImgLoader>(this->_console, entry.path().string())});
        }
        this->_loadImageFiles = false;
    }

    if (ImGui::TreeNodeEx("Images", nodeFlags) && this->_folderPath != "false" && this->_imageFiles.size() > 0) {
        float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        for (size_t cpt = 0; cpt < this->_imageFiles.size(); cpt++) {
            ImGui::PushID(cpt);
            if (ImGui::Button(this->_imageFiles[cpt].name.c_str(), ImVec2(100, 100))) {
                this->_lastMode = this->_mode;
                this->setImage(this->_imageFiles[cpt].path);
                this->switchMode("Image");
            }
            // ! THREAD IT AND YOU GET SOMETHING ! (do thumbnail for raw images, rlly needed here)
            // if (ImGui::ImageButton(this->_imageFiles[cpt].img->getImguiTexture(), ImVec2(100, 100))) {
            //     this->_lastMode = this->_mode;
            //     this->setImage(this->_imageFiles[cpt].path);
            //     this->switchMode("Image");
            // }
            float lastButtonX2 = ImGui::GetItemRectMax().x;
            float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + 100;
            if (cpt + 1 < this->_imageFiles.size() && nextButtonX2 < windowVisibleX)
                ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

void ImgViewer::imageMode()
{
    if (this->_mode != "Image")
        return;

    ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoGridLines;
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    if ((ImPlot::BeginPlot("Image", plotSize, ImPlotFlags_CanvasOnly | ImPlotFlags_Equal)) && this->_image) {
        ImPlot::SetupAxes(0, 0, flags, flags);

        ImVec2 canvasSize = ImPlot::GetPlotPos();
        ImVec2 canvasMax = ImVec2(canvasSize.x + plotSize.x, canvasSize.y + plotSize.y);

        ImPlot::PushPlotClipRect();
        ImPlot::GetPlotDrawList()->AddRectFilled(canvasSize, canvasMax, IM_COL32_BLACK);
        ImPlot::PopPlotClipRect();

        ImPlot::PlotImage("Image", this->_image->getImguiTexture(), ImVec2(0, 0), ImVec2(this->_image->getImgWidth(), this->_image->getImgHeight()));

        ImPlot::EndPlot();
    }

    if (ImGui::Button("Back"))
        this->_mode = _lastMode;
}

void ImgViewer::setImage(std::string imgPath)
{
    this->_imgPath = imgPath;
    this->_image = std::make_shared<ImgLoader>(this->_console, this->_imgPath);

    // ! Cross-platform not compatible
    this->_folderPath = this->_imgPath.substr(0, this->_imgPath.find_last_of("/\\"));
}

void ImgViewer::reloadImages()
{
    this->_loadImageFiles = true;
}

std::string ImgViewer::getFolderPath()
{
    return this->_folderPath;
}

void ImgViewer::frameLoop()
{
    ImGui::Begin("ImgViewer", &this->state);

    this->folderMode();
    this->imageMode();

    ImGui::End();
}

void ImgViewer::initFrame()
{
    this->_imgPath = this->_settings->getSettingsVar("EX4D_ImgViewer_ImagePath");

    if (this->_imgPath != "false" && this->_mode == "Image")
        this->_image = std::make_shared<ImgLoader>(this->_console, this->_imgPath);

    // ! Cross-platform not compatible
    this->_folderPath = this->_imgPath.substr(0, this->_imgPath.find_last_of("/\\"));
}

void ImgViewer::saveSettings()
{
    try {
        this->_settings->setSettingsVar("EX4D_ImgViewer_Mode", this->_mode);
        this->_settings->setSettingsVar("EX4D_ImgViewer_FolderPath", this->_folderPath);
        this->_settings->setSettingsVar("EX4D_ImgViewer_ImagePath", this->_imgPath);
    } catch (std::exception &error) {
        this->_console->throwException(error.what());
    }
}