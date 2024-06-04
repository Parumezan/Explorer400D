#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class Module
    {
        protected:
            std::atomic<bool> _stopSignal = false;
            std::atomic<bool> _threadRunning = false;
            std::atomic<bool> _uiDisable = false;
            std::thread _moduleThread;

        public:
            Module() = default;
            ~Module() = default;

            bool state = false;

            virtual void moduleInit();
            virtual void moduleLoop();
            virtual void moduleClose();

            virtual void moduleSettingsLoad();
            virtual void moduleSettingsSave();

            void moduleThreadInit(std::function<void()> function);
            void moduleThreadLoop();
    };
} // namespace Explorer400D

#endif /* !MODULE_HPP_ */