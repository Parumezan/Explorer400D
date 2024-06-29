#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(Settings &settings, WebFetch &webFetch) : _settings(&settings), _webFetch(&webFetch)
{
}

void Weather::_setLocation()
{
    static char latitude[32] = "", longitude[32] = "", inputLocation[64] = "", name[64] = "", country[64] = "";

    if (this->_locationSet) {
        strcpy(name, this->_location.getName().c_str());
        strcpy(country, this->_location.getCountry().c_str());
        strcpy(latitude, std::to_string(this->_location.getCoordinates().first).c_str());
        strcpy(longitude, std::to_string(this->_location.getCoordinates().second).c_str());
        this->_locationSet = false;
    }

    ImGui::InputText("Search a location", inputLocation, sizeof(inputLocation));

    switch (this->_searchState) {
    case WeatherSearchState::NONE:
        this->_searchStateText = "";
        break;
    case WeatherSearchState::FOUND:
        this->_searchStateText = "Select a location";
        break;
    case WeatherSearchState::BAD:
        this->_searchStateText = "Failed to find location";
        break;
    case WeatherSearchState::SELECTED:
        this->_searchStateText = this->_location.getName() + " (" + this->_location.getCountry() + ")";
        break;
    }

    if (ImGui::BeginCombo("Location", this->_searchStateText.c_str())) {
        for (auto &location : this->_locations)
            if (ImGui::Selectable((location.getName() + " (" + location.getCountry() + ")").c_str())) {
                this->setLocation(location);
                this->_searchState = WeatherSearchState::SELECTED;
            }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Search"))
        this->moduleThreadInit(std::bind(&Weather::_fetchLocationSearch, this, inputLocation));

    ImGui::SameLine();
    if (ImGui::Button("Clear Search")) {
        this->_locations.clear();
        this->_searchState = WeatherSearchState::NONE;
    }

    ImGui::SameLine();
    if (ImGui::Button("Set Location")) {
        if (strlen(latitude) == 0 || strlen(longitude) == 0)
            spdlog::error("Location not set, missing fields");
        else
            this->setLocation(Location(std::stof(latitude), std::stof(longitude), name, country));
    }

    ImGui::SameLine();

    bool unsavedChanges = (strlen(latitude) == 0 || strlen(longitude) == 0);
    if (unsavedChanges || (this->_location.getName() != name || this->_location.getCountry() != country || this->_location.getCoordinates().first != std::stof(latitude) || this->_location.getCoordinates().second != std::stof(longitude))) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unsaved changes");
        this->_fetchable = false;
    } else {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Location saved");
        this->_fetchable = true;
    }

    ImGui::InputText("Name", name, sizeof(name));
    ImGui::InputText("Country", country, sizeof(country));
    ImGui::InputText("Latitude", latitude, sizeof(latitude), ImGuiInputTextFlags_CharsDecimal);
    ImGui::InputText("Longitude", longitude, sizeof(longitude), ImGuiInputTextFlags_CharsDecimal);

    if (ImGui::BeginCombo("Forecast Days", std::to_string(this->_forecastDays).c_str())) {
        for (auto &forecastDay : ForecastDays)
            if (ImGui::Selectable(std::to_string(forecastDay).c_str()))
                this->_forecastDays = forecastDay;
        ImGui::EndCombo();
    }

    if (this->_autoFetch && this->_locations.size() > 0)
        ImGui::SliderInt("10s - 30m", &this->_sliderTime, 10, 1800, "%d");

    if (!this->_fetchable) {
        ImGui::BeginDisabled(true);
        ImGui::Button("Fetch Weather Data");
        ImGui::EndDisabled();
    } else {
        if (ImGui::Button("Fetch Weather Data"))
            this->moduleThreadInit(std::bind(&Weather::_fetchWeatherData, this));
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear Data"))
        for (auto &graph : this->_weatherGraphs)
            graph.clearData();

    ImGui::SameLine();
    if (ImGui::Button("Fit Graphs"))
        this->_initGraphs = true;

    ImGui::SameLine();

    ImGui::Checkbox("Auto Fetch", &this->_autoFetch);
}

void Weather::_fetchLocationSearch(char location[64])
{
    this->_locations.clear();

    std::string locationString = location;
    std::string url = "https://geocoding-api.open-meteo.com/v1/search?name=";
    std::string settings = "&count=10&language=en&format=json";
    std::vector<Location> locations;

    for (auto &character : locationString) {
        if (character == ' ')
            character = '+';
    }

    WebFetchType fetch = this->_webFetch->fetch(url + locationString + settings, WebFetchMethod::FETCH_GET);

    if (fetch.second != 200) {
        this->_searchState = WeatherSearchState::BAD;
        spdlog::error("Failed to fetch location search data, status code: {}", fetch.second);
        return;
    }

    if (std::string::npos == fetch.first.find("results")) {
        this->_searchState = WeatherSearchState::BAD;
        spdlog::error("Failed to parse JSON data");
        return;
    }

    json obj = json::parse(fetch.first);

    for (auto &location : obj["results"]) {
        std::string name = (location["name"].is_null() ? "" : location["name"].get<std::string>());
        std::string country = (location["country"].is_null() ? "" : location["country"].get<std::string>());
        float latitude = (location["latitude"].is_null() ? 0.0f : location["latitude"].get<float>());
        float longitude = (location["longitude"].is_null() ? 0.0f : location["longitude"].get<float>());

        locations.push_back(Location(latitude, longitude, name, country));
    }

    this->_locations = locations;
    this->_searchState = WeatherSearchState::FOUND;
}

void Weather::_fetchWeatherData()
{
    std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(this->_location.getCoordinates().first) + "&longitude=" + std::to_string(this->_location.getCoordinates().second);
    std::string settings = "&timeformat=unixtime&timezone=auto&forecast_days=" + std::to_string(this->_forecastDays);

    for (auto &graph : this->_weatherGraphs) {
        std::string graphSettings = "&hourly=";
        std::map<std::pair<std::string, std::string>, std::vector<double>> data = graph.getData();
        std::vector<double> time;

        for (auto &element : data)
            graphSettings += element.first.second + ",";
        graphSettings.pop_back();

        WebFetchType fetch = this->_webFetch->fetch(url + graphSettings + settings, WebFetchMethod::FETCH_GET);

        if (fetch.second != 200) {
            spdlog::error("Failed to fetch weather forecast data, status code: {}", fetch.second);
            continue;
        }

        json obj = json::parse(fetch.first);

        for (auto &element : obj["hourly"]["time"])
            time.push_back(element);

        std::map<std::pair<std::string, std::string>, std::vector<double>> newData;
        for (auto &element : data)
            for (auto &value : obj["hourly"][element.first.second])
                newData[element.first].push_back(value);

        graph.setData(newData);
        graph.setTime(time);

        this->_initGraphs = true;
    }

    spdlog::info("Weather data fetched");
}

void Weather::setLocation(Location location)
{
    this->_location = location;
    this->_locationSet = true;
}

void Weather::moduleInit()
{
    WeatherGraph cloudCover("Cloud Cover", "Percentage (%)", {0.0, 100.0}, true);

    cloudCover.addData(std::make_pair("Cloud Cover", "cloudcover"));
    cloudCover.addData(std::make_pair("Cloud Cover Low", "cloudcover_low"));
    cloudCover.addData(std::make_pair("Cloud Cover Mid", "cloudcover_mid"));
    cloudCover.addData(std::make_pair("Cloud Cover High", "cloudcover_high"));

    WeatherGraph temperature("Temperature", "Celcius (°C)", {-100.0, 100.0});

    temperature.addData(std::make_pair("Temperature (2m)", "temperature_2m"));
    temperature.addData(std::make_pair("Temp. Apparent", "apparent_temperature"));
    temperature.addData(std::make_pair("Dew Point", "dewpoint_2m"));

    WeatherGraph precipitation("Precipitation", "Probability (%)", {0.0, 100.0});

    precipitation.addData(std::make_pair("Precipitation Probability", "precipitation_probability"));

    WeatherGraph humidity("Humidity", "Percentage (%)", {0.0, 100.0});

    humidity.addData(std::make_pair("Relative Humidity 2m", "relative_humidity_2m"));

    WeatherGraph wind("Wind", "Kilometers per hour (km/h)", {0.0, 10000.0});

    wind.addData(std::make_pair("Wind Speed 180m", "windspeed_180m"));
    wind.addData(std::make_pair("Wind Speed 120m", "windspeed_120m"));
    wind.addData(std::make_pair("Wind Speed 80m", "windspeed_80m"));
    wind.addData(std::make_pair("Wind Speed 10m", "windspeed_10m"));
    wind.addData(std::make_pair("Wind Gusts 10m", "windgusts_10m"));

    this->_weatherGraphs.push_back(cloudCover);
    this->_weatherGraphs.push_back(temperature);
    this->_weatherGraphs.push_back(precipitation);
    this->_weatherGraphs.push_back(humidity);
    this->_weatherGraphs.push_back(wind);
}

void Weather::moduleLoop()
{
    ImGui::Begin("Weather", &this->state);

    ImGui::BeginDisabled(this->_uiDisable);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx("Set location", nodeFlags)) {
        this->_setLocation();
        ImGui::TreePop();
    }

    ImGui::SeparatorText("Weather Data");

    for (auto &graph : this->_weatherGraphs) {
        if (this->_initGraphs)
            ImPlot::SetNextAxesToFit();
        if (ImGui::TreeNodeEx(graph.getName().c_str(), nodeFlags)) {
            graph.drawGraph();
            ImGui::TreePop();
        }
    }
    if (this->_initGraphs)
        this->_initGraphs = false;

    if (this->_autoFetch) {
        if (this->_lastFetch == 0 || std::time(0) - this->_lastFetch > this->_sliderTime)
            this->moduleThreadInit(std::bind(&Weather::_fetchWeatherData, this));
    }

    ImGui::EndDisabled();

    ImGui::End();
}

void Weather::moduleClose()
{
    this->_weatherGraphs.clear();
}

void Weather::moduleSettingsLoad()
{
    json obj = nullptr;

    (obj = this->_settings->getSetting("Explorer400D::Weather::State")) != nullptr ? this->state = obj.get<bool>() : this->state = false;
    (obj = this->_settings->getSetting("Explorer400D::Weather::Location")) != nullptr ? this->setLocation(Location(obj)) : this->setLocation(Location());
    (obj = this->_settings->getSetting("Explorer400D::Weather::ForecastDays")) != nullptr ? this->_forecastDays = obj.get<int>() : this->_forecastDays = 3;
}

void Weather::moduleSettingsSave()
{
    this->_settings->setSetting("Explorer400D::Weather::State", this->state);
    this->_settings->setSetting("Explorer400D::Weather::Location", this->_location.getJson());
    this->_settings->setSetting("Explorer400D::Weather::ForecastDays", this->_forecastDays);
}