#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class Module
    {
        public:
            Module() = default;
            ~Module() = default;

            bool state = false;

            virtual void moduleInit();
            virtual void moduleLoop();
            virtual void moduleClose();

            virtual void moduleLoadSettings();
            virtual void moduleSaveSettings();
    };
} // namespace Explorer400D

#endif /* !MODULE_HPP_ */