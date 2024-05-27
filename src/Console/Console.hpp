#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include "Includes.hpp"
#include "Module/Module.hpp"

namespace Explorer400D
{
    class Console : public Module
    {
        public:
            Console() = default;
            ~Console() = default;
            void moduleInit() override;
            void moduleLoop() override;
    };
}

#endif /* !CONSOLE_HPP_ */
