#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(std::shared_ptr<Console> console) : ChildConsole(console)
{
    this->_curl = curl_easy_init();
    if (!this->_curl)
        this->_console->warn("Failed to initialize libcurl");
    this->_urlClouds = "https://api.open-meteo.com/v1/forecast?latitude=48.112&longitude=-1.6743&hourly=cloudcover,cloudcover_low,cloudcover_mid,cloudcover_high,visibility&timezone=Europe%2FBerlin";
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

void Weather::getWeather()
{
    this->_console->info("Refreshing weather...");
    if (!this->_curl) {
        this->_console->warn("Failed to refresh weather");
        return;
    }

    // Set URL
    curl_easy_setopt(this->_curl, CURLOPT_URL, this->_urlClouds.c_str());

    // Set callback function
    curl_easy_setopt(this->_curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // Set callback data
    std::string readBuffer;
    curl_easy_setopt(this->_curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    CURLcode result = curl_easy_perform(this->_curl);
    if (result != CURLE_OK) {
        this->_console->warn("Failed to refresh weather");
        return;
    }

    this->_console->info("Weather refreshed");
    this->_console->info(readBuffer);
}

void Weather::frameLoop()
{
    ImGui::Begin("Weather");
    if (ImGui::Button("Refresh")) {
        this->getWeather();
    }
    ImGui::End();
}