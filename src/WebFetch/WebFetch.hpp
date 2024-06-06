#ifndef WEBFETCH_HPP_
#define WEBFETCH_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"

#define WebFetchType std::pair<std::string, long>

namespace Explorer400D
{
    enum class WebFetchMethod {
        FETCH_GET,
        FETCH_POST,
        FETCH_PUT,
        FETCH_DELETE
    };

    class WebFetch : public Module
    {
        private:
            static size_t _writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp);

        public:
            WebFetch() = default;
            ~WebFetch() = default;

            WebFetchType fetch(const std::string &url, WebFetchMethod method, const std::string &postFields = "");

            void moduleInit() override;
            void moduleClose() override;
    };
} // namespace Explorer400D

#endif /* !WEBFETCH_HPP_ */
