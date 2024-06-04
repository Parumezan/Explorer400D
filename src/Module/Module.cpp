#include "Module.hpp"

using namespace Explorer400D;

void Module::moduleInit()
{
}

void Module::moduleLoop()
{
}

void Module::moduleClose()
{
}

void Module::moduleSettingsLoad()
{
}

void Module::moduleSettingsSave()
{
}

void Module::moduleThreadInit(std::function<void()> function)
{
    if (!this->_threadRunning.load()) {
        this->_moduleThread = std::thread([this, function]() {
            this->_threadRunning.store(true);
            this->_uiDisable.store(true);
            function();
            this->_uiDisable.store(false);
            this->_stopSignal.store(true);
        });
    }
}

void Module::moduleThreadLoop()
{
    if (this->_stopSignal.load()) {
        this->_threadRunning.store(false);
        this->_stopSignal.store(false);
        this->_moduleThread.join();
    }
}