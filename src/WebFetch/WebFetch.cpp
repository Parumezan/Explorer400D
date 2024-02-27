#include "WebFetch.hpp"

using namespace Explorer400D;

WebFetch::WebFetch(std::shared_ptr<Console> console) : _console(console)
{
    this->_console->info("WebFetch module started");
}

WebFetch::~WebFetch()
{
}

std::string WebFetch::fetchData(std::string url)
{
    std::stringstream response;

    try {
        _request.setOpt(new curlpp::options::Url(url));
        _request.setOpt(new curlpp::options::WriteStream(&response));
        _request.perform();
        _request.reset();
    } catch (const std::exception &error) {
        this->_console->error(error.what());
    }
    this->_console->info("Fetched data from " + url);
    return response.str();
}

void WebFetch::fetchImg(std::string filePath, std::string url)
{
    std::ofstream outFile(filePath, std::ofstream::binary);

    try {
        _request.setOpt(new curlpp::options::Url(url));
        _request.setOpt(new curlpp::options::WriteStream(&outFile));
        _request.perform();
        _request.reset();
    } catch (const std::exception &error) {
        this->_console->error(error.what());
    }
    this->_console->info("Fetched image from " + url + " to " + filePath);
}