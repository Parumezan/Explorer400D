#include "WebFetch.hpp"

using namespace Explorer400D;

size_t WebFetch::_writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

WebFetchType WebFetch::fetch(const std::string &url, WebFetchMethod method, const std::string &postFields)
{
    CURL *curl;
    CURLcode res;

    std::string readBuffer = "";
    long httpCode = -1;

    spdlog::info("Fetching URL: {}", url);

    if (!(curl = curl_easy_init())) {
        spdlog::error("curl_easy_init() failed");
        return {readBuffer, httpCode};
    }

    spdlog::info("curl_easy_init() success");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    spdlog::info("curl_easy_setopt(1) success");

    switch (method) {
    case WebFetchMethod::FETCH_GET:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        break;
    case WebFetchMethod::FETCH_POST:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        break;
    case WebFetchMethod::FETCH_PUT:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        break;
    case WebFetchMethod::FETCH_DELETE:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    default:
        method = WebFetchMethod::FETCH_GET;
    }

    spdlog::info("curl_easy_setopt(2) success");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    spdlog::info("curl_easy_setopt(3) success");

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));

    spdlog::info("curl_easy_perform() success");

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    spdlog::info("curl_easy_getinfo() success");

    curl_easy_cleanup(curl);

    spdlog::info("curl_easy_cleanup() success");

    return {readBuffer, httpCode};
}

void WebFetch::moduleInit()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void WebFetch::moduleClose()
{
    curl_global_cleanup();
}