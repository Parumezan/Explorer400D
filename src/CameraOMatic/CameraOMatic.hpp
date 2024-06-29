#ifndef CAMERAOMATIC_HPP_
#define CAMERAOMATIC_HPP_

#include "CameraManager/CameraSettings/CameraSettings.hpp"
#include "Includes.hpp"
#include "Module/Module.hpp"
#include "Settings/Settings.hpp"

namespace Explorer400D
{
    class CameraOMatic : public Module
    {
        private:
            Settings *_settings;

        public:
            CameraOMatic(Settings &settings);
            ~CameraOMatic() = default;

            void moduleInit() override;
            void moduleLoop() override;
            void moduleClose() override;

            void moduleSettingsLoad() override;
            void moduleSettingsSave() override;
    };
} // namespace Explorer400D

#endif /* !CAMERAOMATIC_HPP_ */
