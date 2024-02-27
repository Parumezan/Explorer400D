#include "Moon.hpp"

using namespace Explorer400D;

Moon::Moon(std::shared_ptr<Console> console, std::shared_ptr<Settings> settings) : _console(console), _settings(settings)
{
    this->_console->info("Initializing Moon module...");
    try {
        this->_webFetch = std::make_shared<WebFetch>(this->_console);
        this->_moonData.time = "";
        this->_lastFetch = 0;
    } catch (std::exception &error) {
        this->_console->error(error.what());
    }
    this->_console->info("Moon module initialized");
}

Moon::~Moon()
{
}

void Moon::initMoonData(std::string stringMoonData)
{
    nlohmann::json moonData = nlohmann::json::parse(stringMoonData);

    MoonData_t data;

    data.time = moonData["time"];
    data.phase = moonData["phase"];
    data.obscuration = moonData["obscuration"];
    data.age = moonData["age"];
    data.diameter = moonData["diameter"];
    data.distance = moonData["distance"];
    data.j2000_ra = moonData["j2000_ra"];
    data.j2000_dec = moonData["j2000_dec"];
    data.subsolar_lon = moonData["subsolar_lon"];
    data.subsolar_lat = moonData["subsolar_lat"];
    data.subearth_lon = moonData["subearth_lon"];
    data.subearth_lat = moonData["subearth_lat"];
    data.posangle = moonData["posangle"];

    this->_moonData = data;
    this->_console->info("Moon data initialized");
}

void Moon::fetchMoonImg(std::string url)
{
    std::string filePath = this->_settings->getSettingsPath() + "/moon.jpg";

    this->_webFetch->fetchImg(filePath, url);
    this->_moonImg = std::make_shared<ImgLoader>(this->_console, filePath);
    this->_console->info("Moon image fetch & loaded");
}

void Moon::fetchMoonData(bool highres)
{
    std::string url = "https://svs.gsfc.nasa.gov/api/dialamoon/";
    std::string timeNow = this->_console->getIsoTime();
    timeNow.erase(timeNow.end() - 3, timeNow.end());
    url += timeNow;

    this->_lastFetch = std::time(0);
    std::string moonData = this->_webFetch->fetchData(url);
    if (moonData.empty())
        throw this->_console->throwException("Failed to fetch moon data");

    nlohmann::json jsonMoonData = nlohmann::json::parse(moonData);

    if (highres)
        this->fetchMoonImg(jsonMoonData["image_highres"]["url"]);
    else
        this->fetchMoonImg(jsonMoonData["image"]["url"]);

    this->initMoonData(moonData);
}

void Moon::showMoonData()
{
    /* Data :
    "time": "2023-08-11T18:00",
    "phase": 18.91,
    "obscuration": 0.0,
    "age": 24.978,
    "diameter": 1798.1,
    "distance": 398596.0,
    "j2000_ra": 5.789,
    "j2000_dec": 27.7649,
    "subsolar_lon": -122.725,
    "subsolar_lat": 1.447,
    "subearth_lon": 5.781,
    "subearth_lat": -5.72,
    "posangle": 359.522
    */

    /* Data to show :
    Time (UTC)	8/11/2023, 6:00:00 PM
    Phase	18.91% (24d 23h 28m)
    Diameter	1798.1 arcseconds
    Distance	398596 km (31.24 Earth diameter(s))
    J2000 Right Ascension, Declination	5h 47m 20s, 28° 46' 54"
    Sub-Solar Longitude, Latitude	-122.725°, 1.447°
    Sub-Earth Longitude, Latitude	5.781°, -5.72°
    Position Angle	359.522°
    */

    ImGui::Text("Time (UTC) : %s", this->_moonData.time.c_str());
    ImGui::Text("Phase : %.2f%% (%.2fd %.2fh %.2fm)", this->_moonData.phase, this->_moonData.age, (this->_moonData.age - (int)this->_moonData.age) * 24, ((this->_moonData.age - (int)this->_moonData.age) * 24 - (int)((this->_moonData.age - (int)this->_moonData.age) * 24)) * 60);
    ImGui::Text("Diameter : %.2f arcseconds", this->_moonData.diameter);
    ImGui::Text("Distance : %.2f km (%.2f Earth diameter(s))", this->_moonData.distance, this->_moonData.distance / 12742);
    ImGui::Text("J2000 Right Ascension, Declination : %.2fh %.2fm %.2fs, %.2f° %.2f' %.2f\"", this->_moonData.j2000_ra, (this->_moonData.j2000_ra - (int)this->_moonData.j2000_ra) * 60,
                ((this->_moonData.j2000_ra - (int)this->_moonData.j2000_ra) * 60 - (int)((this->_moonData.j2000_ra - (int)this->_moonData.j2000_ra) * 60)) * 60, this->_moonData.j2000_dec, (this->_moonData.j2000_dec - (int)this->_moonData.j2000_dec) * 60,
                ((this->_moonData.j2000_dec - (int)this->_moonData.j2000_dec) * 60 - (int)((this->_moonData.j2000_dec - (int)this->_moonData.j2000_dec) * 60)) * 60);
    ImGui::Text("Sub-Solar Longitude, Latitude : %.2f°, %.2f°", this->_moonData.subsolar_lon, this->_moonData.subsolar_lat);
    ImGui::Text("Sub-Earth Longitude, Latitude : %.2f°, %.2f°", this->_moonData.subearth_lon, this->_moonData.subearth_lat);
    ImGui::Text("Position Angle : %.2f°", this->_moonData.posangle);
}

void Moon::frameLoop()
{
    ImGui::Begin("Moon", &this->state);

    ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoGridLines;
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    // Plot the moon image
    if ((ImPlot::BeginPlot("Moon", plotSize, ImPlotFlags_CanvasOnly | ImPlotFlags_Equal)) && this->_moonImg) {
        ImPlot::SetupAxes(0, 0, flags, flags);

        ImVec2 canvasSize = ImPlot::GetPlotPos();
        ImVec2 canvasMax = ImVec2(canvasSize.x + plotSize.x, canvasSize.y + plotSize.y);

        ImPlot::PushPlotClipRect();
        ImPlot::GetPlotDrawList()->AddRectFilled(canvasSize, canvasMax, IM_COL32_BLACK);
        ImPlot::PopPlotClipRect();

        ImPlot::PlotImage("Moon", this->_moonImg->getImguiTexture(), ImVec2(0, 0), ImVec2(this->_moonImg->getImgWidth(), this->_moonImg->getImgHeight()));

        ImPlot::EndPlot();
    }

    // Fetch moon data button
    if (ImGui::Button("Fetch Moon")) {
        try {
            this->fetchMoonData(false);
        } catch (std::exception &error) {
            this->_console->error(error.what());
        }
    }

    // ! Highres moon is a tiff file, not supported yet
    // ImGui::SameLine();
    //
    // // Fetch moon highres data button
    // if (ImGui::Button("Fetch Moon Highres")) {
    //     try {
    //         this->fetchMoonData(true);
    //     } catch (std::exception &error) {
    //         this->_console->error(error.what());
    //     }
    // }

    // Show moon data
    if (this->_moonData.time != "") {
        try {
            this->showMoonData();
        } catch (std::exception &error) {
            this->_console->error(error.what());
        }
    }

    // Fetch moon data all hours
    if (this->_lastFetch == 0 || std::time(0) - this->_lastFetch > 3600) {
        try {
            this->fetchMoonData(false);
        } catch (std::exception &error) {
            this->_console->error(error.what());
        }
    }

    ImGui::End();
}