#ifndef SELFGUIDING_HPP_
#define SELFGUIDING_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"
#include "WebFetch/WebFetch.hpp"

namespace Explorer400D
{
    class SelfGuiding : public Module
    {
        private:
            Settings *_settings;
            WebFetch *_webFetch;

        public:
            SelfGuiding(Settings &settings, WebFetch &webFetch);
            ~SelfGuiding() = default;

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
}

#endif /* !SELFGUIDING_HPP_ */
