#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(std::shared_ptr<Console> console) : ChildConsole(console)
{
    bool isStarted = true;
    this->_console->info("Initializing weather module...");
    this->_selectedLocation.name = "Select a location";
    this->_curl = curl_easy_init();
    if (!this->_curl) {
        this->_console->warn("Failed to initialize libcurl");
        isStarted = false;
    }
    if (isStarted) {
        this->_console->info("Weather module started");
    } else {
        this->_console->warn("Weather module encountered difficulties");
    }
}

Weather::~Weather()
{
    curl_easy_cleanup(this->_curl);
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t totalSize = size * nmemb;
    output->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

std::string Weather::fetchData(std::string url)
{
    this->_console->info("Try to fetch data from " + url);
    if (!this->_curl) {
        this->_console->warn("Failed to fetch data, libcurl is not initialized");
        return "";
    }

    // Set URL
    curl_easy_setopt(this->_curl, CURLOPT_URL, url.c_str());

    // Set callback function
    curl_easy_setopt(this->_curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // Set callback data
    std::string readBuffer;
    curl_easy_setopt(this->_curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    CURLcode result = curl_easy_perform(this->_curl);
    if (result != CURLE_OK) {
        this->_console->warn("Failed to fetch data");
        return "";
    }
    this->_console->info("Data fetched from " + url);
    return readBuffer;
}

void Weather::fetchLocationSearch(char location[64])
{
    std::string locationString = location;
    std::string url = "https://geocoding-api.open-meteo.com/v1/search?name=";
    std::string settings = "&count=10&language=en&format=json";
    std::string data;
    std::vector<Location_t> locations;

    for (auto &character : locationString) {
        if (character == ' ')
            character = '+';
    }

    data = this->fetchData(url + locationString + settings);

    if (std::string::npos == data.find("results"))
        throw this->_console->throwException("Failed to parse JSON data");

    nlohmann::json json = nlohmann::json::parse(data);

    for (auto &element : json["results"]) {
        Location_t location;
        location.name = element["name"];
        location.latitude = element["latitude"];
        location.longitude = element["longitude"];
        location.country = element["country"];
        locations.push_back(location);
    }
    this->_locations = locations;
}

void Weather::fetchWeather()
{
    std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(this->_selectedLocation.latitude) + "&longitude=" + std::to_string(this->_selectedLocation.longitude);
    std::string settings = "&timeformat=unixtime&timezone=auto&forecast_days=7";
    std::string urlClouds = url + "&hourly=cloudcover,cloudcover_low,cloudcover_mid,cloudcover_high" + settings;

    std::string dataClouds = this->fetchData(urlClouds);
    if (dataClouds.empty())
        throw this->_console->throwException("Failed to fetch weather data");

    nlohmann::json jsonClouds = nlohmann::json::parse(dataClouds);

    std::vector<double> time;
    std::vector<double> cloudCover;
    std::vector<double> cloudCoverLow;
    std::vector<double> cloudCoverMid;
    std::vector<double> cloudCoverHigh;

    for (auto &element : jsonClouds["hourly"]["time"])
        time.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover"])
        cloudCover.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover_low"])
        cloudCoverLow.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover_mid"])
        cloudCoverMid.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover_high"])
        cloudCoverHigh.push_back(element);

    this->_time = time;
    this->_cloudCover = cloudCover;
    this->_cloudCoverLow = cloudCoverLow;
    this->_cloudCoverMid = cloudCoverMid;
    this->_cloudCoverHigh = cloudCoverHigh;
}

void Weather::drawClouds()
{
    ImPlot::GetStyle().UseLocalTime;
    if (ImPlot::BeginPlot("Cloud Cover", ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
        if (this->_cloudCover.size() > 0) {
            ImPlot::SetupAxes("Hours", "Cloud Cover in %", 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxesLimits(this->_time[0], this->_time[this->_time.size() - 1], 0, 100);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, this->_time[0], this->_time[this->_time.size() - 1]);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 100);

            ImPlot::PlotLine("Cloud Cover", this->_time.data(), this->_cloudCover.data(), this->_time.size());
            ImPlot::PlotLine("Cloud Cover Low", this->_time.data(), this->_cloudCoverLow.data(), this->_time.size());
            ImPlot::PlotLine("Cloud Cover Mid", this->_time.data(), this->_cloudCoverMid.data(), this->_time.size());
            ImPlot::PlotLine("Cloud Cover High", this->_time.data(), this->_cloudCoverHigh.data(), this->_time.size());

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.50f);

            ImPlot::PlotShaded("Cloud Cover", this->_time.data(), this->_cloudCover.data(), this->_time.size(), -INFINITY);
            ImPlot::PlotShaded("Cloud Cover Low", this->_time.data(), this->_cloudCoverLow.data(), this->_time.size(), -INFINITY);
            ImPlot::PlotShaded("Cloud Cover Mid", this->_time.data(), this->_cloudCoverMid.data(), this->_time.size(), -INFINITY);
            ImPlot::PlotShaded("Cloud Cover High", this->_time.data(), this->_cloudCoverHigh.data(), this->_time.size(), -INFINITY);
        }
        ImPlot::EndPlot();
    }
}

void Weather::frameLoop()
{
    static char latitude[32] = "", longitude[32] = "";
    static char location[64] = "";

    ImGui::Begin("Weather", &this->state);

    ImGui::InputText("", location, sizeof(location));
    ImGui::SameLine();
    if (ImGui::Button("Search Location")) {
        try {
            this->fetchLocationSearch(location);
        } catch (std::exception &error) {
        }
    }

    if (ImGui::BeginCombo("Locations", this->_selectedLocation.name.c_str())) {
        for (auto &location : this->_locations) {
            if (ImGui::Selectable((location.name + " (" + location.country + ")").c_str())) {
                this->_console->info("Selected location: " + location.name + " (" + location.country + ")");
                this->_selectedLocation = location;
                try {
                    std::memset(latitude, 0, sizeof(latitude));
                    std::memset(longitude, 0, sizeof(longitude));
                    std::strcat(latitude, std::to_string(location.latitude).c_str());
                    std::strcat(longitude, std::to_string(location.longitude).c_str());
                } catch (std::exception &error) {
                }
            }
        }
        ImGui::EndCombo();
    }

    ImGui::InputText("Latitude", latitude, sizeof(latitude), ImGuiInputTextFlags_CharsDecimal);
    ImGui::InputText("Longitude", longitude, sizeof(longitude), ImGuiInputTextFlags_CharsDecimal);

    if (ImGui::Button("Fetch Weather")) {
        this->_selectedLocation.latitude = std::stof(latitude);
        this->_selectedLocation.longitude = std::stof(longitude);
        try {
            ImPlot::SetNextAxesToFit();
            this->fetchWeather();
        } catch (std::exception &error) {
        }
    }

    this->drawClouds();

    ImGui::End();
}