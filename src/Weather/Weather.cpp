#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(std::shared_ptr<Console> console) : ChildConsole(console)
{
    bool isStarted = true;
    this->_console->info("Initializing weather module...");
    this->_isFit = true;
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

    if (std::string::npos == data.find("results")) {
        this->_selectedLocationName = "Failed to find location";
        throw this->_console->throwException("Failed to parse JSON data");
    }

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
    this->_selectedLocationName = "Select a location";
}

void Weather::fetchClouds(std::string url, std::string settings)
{
    std::string urlClouds = url + "&hourly=cloudcover,cloudcover_low,cloudcover_mid,cloudcover_high" + settings;

    std::string dataClouds = this->fetchData(urlClouds);
    if (dataClouds.empty())
        throw this->_console->throwException("Failed to fetch cloudcover weather data");

    nlohmann::json jsonClouds = nlohmann::json::parse(dataClouds);

    PlotClouds_t plotClouds;

    for (auto &element : jsonClouds["hourly"]["time"])
        plotClouds.time.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover"])
        plotClouds.cloudCover.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover_low"])
        plotClouds.cloudCoverLow.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover_mid"])
        plotClouds.cloudCoverMid.push_back(element);
    for (auto &element : jsonClouds["hourly"]["cloudcover_high"])
        plotClouds.cloudCoverHigh.push_back(element);

    this->_plotClouds = plotClouds;
}

void Weather::fetchTemperature(std::string url, std::string settings)
{
    std::string urlTemp = url + "&hourly=temperature_2m,dewpoint_2m,apparent_temperature" + settings;

    std::string dataTemp = this->fetchData(urlTemp);
    if (dataTemp.empty())
        throw this->_console->throwException("Failed to fetch temperature weather data");

    nlohmann::json jsonTemp = nlohmann::json::parse(dataTemp);

    PlotTemperature_t plotTemp;

    for (auto &element : jsonTemp["hourly"]["time"])
        plotTemp.time.push_back(element);
    for (auto &element : jsonTemp["hourly"]["temperature_2m"])
        plotTemp.temp.push_back(element);
    for (auto &element : jsonTemp["hourly"]["dewpoint_2m"])
        plotTemp.tempDewPoint.push_back(element);
    for (auto &element : jsonTemp["hourly"]["apparent_temperature"])
        plotTemp.tempApparent.push_back(element);

    this->_plotTemp = plotTemp;
}

void Weather::fetchPrecipitation(std::string url, std::string settings)
{
    std::string urlPreci = url + "&hourly=precipitation_probability" + settings;

    std::string dataPreci = this->fetchData(urlPreci);
    if (dataPreci.empty())
        throw this->_console->throwException("Failed to fetch precipitation weather data");

    nlohmann::json jsonPreci = nlohmann::json::parse(dataPreci);

    PlotPrecipitation_t plotPreci;

    for (auto &element : jsonPreci["hourly"]["time"])
        plotPreci.time.push_back(element);
    for (auto &element : jsonPreci["hourly"]["precipitation_probability"])
        plotPreci.preciProbability.push_back(element);

    this->_plotPreci = plotPreci;
}

void Weather::fetchWind(std::string url, std::string settings)
{
    std::string urlWind = url + "&hourly=windspeed_10m,windspeed_80m,windspeed_120m,windspeed_180m,windgusts_10m" + settings;

    std::string dataWind = this->fetchData(urlWind);
    if (dataWind.empty())
        throw this->_console->throwException("Failed to fetch wind weather data");

    nlohmann::json jsonWind = nlohmann::json::parse(dataWind);

    PlotWind_t plotWind;

    for (auto &element : jsonWind["hourly"]["time"])
        plotWind.time.push_back(element);
    for (auto &element : jsonWind["hourly"]["windgusts_10m"])
        plotWind.windGusts10m.push_back(element);
    for (auto &element : jsonWind["hourly"]["windspeed_10m"])
        plotWind.windSpeed10m.push_back(element);
    for (auto &element : jsonWind["hourly"]["windspeed_80m"])
        plotWind.windSpeed80m.push_back(element);
    for (auto &element : jsonWind["hourly"]["windspeed_120m"])
        plotWind.windSpeed120m.push_back(element);
    for (auto &element : jsonWind["hourly"]["windspeed_180m"])
        plotWind.windSpeed180m.push_back(element);

    this->_plotWind = plotWind;
}

void Weather::fetchWeather()
{
    std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(this->_selectedLocation.latitude) + "&longitude=" + std::to_string(this->_selectedLocation.longitude);
    std::string settings = "&timeformat=unixtime&timezone=auto&forecast_days=7";

    this->fetchClouds(url, settings);
    this->fetchTemperature(url, settings);
    this->fetchPrecipitation(url, settings);
    this->fetchWind(url, settings);
}

void Weather::drawClouds()
{
    if (ImPlot::BeginPlot("Cloud Cover", ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
        if (this->_plotClouds.cloudCover.size() > 0) {
            ImPlot::SetupAxes("Time", "Cloud Cover in %", 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxesLimits(this->_plotClouds.time[0], this->_plotClouds.time[this->_plotClouds.time.size() - 1], 0, 100);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, this->_plotClouds.time[0], this->_plotClouds.time[this->_plotClouds.time.size() - 1]);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 100);

            ImPlot::PlotLine("Cloud Cover", this->_plotClouds.time.data(), this->_plotClouds.cloudCover.data(), this->_plotClouds.time.size());
            ImPlot::PlotLine("Cloud Cover Low", this->_plotClouds.time.data(), this->_plotClouds.cloudCoverLow.data(), this->_plotClouds.time.size());
            ImPlot::PlotLine("Cloud Cover Mid", this->_plotClouds.time.data(), this->_plotClouds.cloudCoverMid.data(), this->_plotClouds.time.size());
            ImPlot::PlotLine("Cloud Cover High", this->_plotClouds.time.data(), this->_plotClouds.cloudCoverHigh.data(), this->_plotClouds.time.size());

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.50f);

            ImPlot::PlotShaded("Cloud Cover", this->_plotClouds.time.data(), this->_plotClouds.cloudCover.data(), this->_plotClouds.time.size(), -INFINITY);
            ImPlot::PlotShaded("Cloud Cover Low", this->_plotClouds.time.data(), this->_plotClouds.cloudCoverLow.data(), this->_plotClouds.time.size(), -INFINITY);
            ImPlot::PlotShaded("Cloud Cover Mid", this->_plotClouds.time.data(), this->_plotClouds.cloudCoverMid.data(), this->_plotClouds.time.size(), -INFINITY);
            ImPlot::PlotShaded("Cloud Cover High", this->_plotClouds.time.data(), this->_plotClouds.cloudCoverHigh.data(), this->_plotClouds.time.size(), -INFINITY);

            double actualTime = (double)std::time(0);
            ImPlot::PlotInfLines("Now", &actualTime, 1);
        }
        ImPlot::EndPlot();
    }
}

void Weather::drawTemp()
{
    if (ImPlot::BeginPlot("Temperature", ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
        if (this->_plotTemp.temp.size() > 0) {
            ImPlot::SetupAxes("Time", "Celsius (°C)", 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxesLimits(this->_plotTemp.time[0], this->_plotTemp.time[this->_plotTemp.time.size() - 1], -100, 100);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, this->_plotTemp.time[0], this->_plotTemp.time[this->_plotTemp.time.size() - 1]);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -100, 100);

            ImPlot::PlotLine("Temperature (2m)", this->_plotTemp.time.data(), this->_plotTemp.temp.data(), this->_plotTemp.time.size());
            ImPlot::PlotLine("Temp. Apparent", this->_plotTemp.time.data(), this->_plotTemp.tempApparent.data(), this->_plotTemp.time.size());
            ImPlot::PlotLine("Dewpoint (2m)", this->_plotTemp.time.data(), this->_plotTemp.tempDewPoint.data(), this->_plotTemp.time.size());

            double actualTime = (double)std::time(0);
            ImPlot::PlotInfLines("Now", &actualTime, 1);
        }
        ImPlot::EndPlot();
    }
}

void Weather::drawPreci()
{
    if (ImPlot::BeginPlot("Precipitation", ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
        if (this->_plotPreci.preciProbability.size() > 0) {
            ImPlot::SetupAxes("Time", "Probability in %", 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxesLimits(this->_plotPreci.time[0], this->_plotPreci.time[this->_plotPreci.time.size() - 1], 0, 100);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, this->_plotPreci.time[0], this->_plotPreci.time[this->_plotPreci.time.size() - 1]);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 100);

            ImPlot::PlotLine("Precipitation Probability", this->_plotPreci.time.data(), this->_plotPreci.preciProbability.data(), this->_plotPreci.time.size());

            double actualTime = (double)std::time(0);
            ImPlot::PlotInfLines("Now", &actualTime, 1);
        }
        ImPlot::EndPlot();
    }
}

void Weather::drawWind()
{
    if (ImPlot::BeginPlot("Wind", ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
        if (this->_plotWind.windSpeed10m.size() > 0) {
            ImPlot::SetupAxes("Time", "Wind Speed in hm/h", 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxesLimits(this->_plotWind.time[0], this->_plotWind.time[this->_plotWind.time.size() - 1], 0, 10000);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, this->_plotWind.time[0], this->_plotWind.time[this->_plotWind.time.size() - 1]);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 10000);

            ImPlot::PlotLine("Wind Speed 180m", this->_plotWind.time.data(), this->_plotWind.windSpeed180m.data(), this->_plotWind.time.size());
            ImPlot::PlotLine("Wind Speed 120m", this->_plotWind.time.data(), this->_plotWind.windSpeed120m.data(), this->_plotWind.time.size());
            ImPlot::PlotLine("Wind Speed 80m", this->_plotWind.time.data(), this->_plotWind.windSpeed80m.data(), this->_plotWind.time.size());
            ImPlot::PlotLine("Wind Speed 10m", this->_plotWind.time.data(), this->_plotWind.windSpeed10m.data(), this->_plotWind.time.size());
            ImPlot::PlotLine("Wind Gusts 10m", this->_plotWind.time.data(), this->_plotWind.windGusts10m.data(), this->_plotWind.time.size());

            double actualTime = (double)std::time(0);
            ImPlot::PlotInfLines("Now", &actualTime, 1);
        }
        ImPlot::EndPlot();
    }
}

void Weather::setLocation()
{
    static char latitude[32] = "", longitude[32] = "";
    static char inputLocation[64] = "";

    ImGui::InputText("", inputLocation, sizeof(inputLocation));
    ImGui::SameLine();
    if (ImGui::Button("Search Location")) {
        try {
            this->fetchLocationSearch(inputLocation);
        } catch (std::exception &error) {
        }
    }

    if (ImGui::BeginCombo("Locations", this->_selectedLocationName.c_str())) {
        for (auto &location : this->_locations) {
            if (ImGui::Selectable((location.name + " (" + location.country + ")").c_str())) {
                this->_console->info("Selected location: " + location.name + " (" + location.country + ")");
                this->_selectedLocation = location;
                this->_selectedLocationName = location.name + " (" + location.country + ")";
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
        try {
            this->_selectedLocation.latitude = std::stof(latitude);
            this->_selectedLocation.longitude = std::stof(longitude);
            this->fetchWeather();
            this->_isFit = false;
        } catch (std::exception &error) {
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        this->_plotClouds = {};
        this->_plotTemp = {};
        this->_plotPreci = {};
        this->_plotWind = {};
    }
}

void Weather::frameLoop()
{
    ImGui::Begin("Weather", &this->state);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Set location", nodeFlags)) {
        this->setLocation();
        ImGui::TreePop();
    }

    ImGui::SeparatorText("Weather Data");
    if (this->_isFit) {
        if (ImGui::TreeNodeEx("Temperature", nodeFlags)) {
            this->drawTemp();
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Cloud Cover", nodeFlags)) {
            this->drawClouds();
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Precipitation", nodeFlags)) {
            this->drawPreci();
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Wind", nodeFlags)) {
            this->drawWind();
            ImGui::TreePop();
        }
    } else {
        ImPlot::SetNextAxesToFit();
        if (ImGui::TreeNodeEx("Temperature", nodeFlags)) {
            this->drawTemp();
            ImGui::TreePop();
        }
        ImPlot::SetNextAxesToFit();
        if (ImGui::TreeNodeEx("Cloud Cover", nodeFlags)) {
            this->drawClouds();
            ImGui::TreePop();
        }
        ImPlot::SetNextAxesToFit();
        if (ImGui::TreeNodeEx("Precipitation", nodeFlags)) {
            this->drawPreci();
            ImGui::TreePop();
        }
        ImPlot::SetNextAxesToFit();
        if (ImGui::TreeNodeEx("Wind", nodeFlags)) {
            this->drawWind();
            ImGui::TreePop();
        }
        this->_isFit = true;
    }

    ImGui::End();
}