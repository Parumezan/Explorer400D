#include "Weather.hpp"

using namespace Explorer400D;

Weather::Weather(std::shared_ptr<Console> console) : ChildConsole(console)
{
    bool isStarted = true;
    this->_console->info("Initializing weather module...");
    this->_apiUrl = "https://api.open-meteo.com/v1/";
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

void Weather::frameLoop()
{
    static char latitude[32] = "", longitude[32] = "";

    ImGui::Begin("Weather", &this->state);

    ImGui::InputText("Latitude", latitude, sizeof(latitude), ImGuiInputTextFlags_CharsDecimal);
    ImGui::InputText("Longitude", longitude, sizeof(longitude), ImGuiInputTextFlags_CharsDecimal);
    // if (ImGui::BeginTable("GetLocation", 2, ImGuiTableFlags_Borders)) {
    //     ImGui::TableNextRow();
    //     ImGui::EndTable();
    // }

    ImGui::End();
}