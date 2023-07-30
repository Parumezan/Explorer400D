#ifndef CHILDCONSOLE_HPP_
#define CHILDCONSOLE_HPP_

#include "Console.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class ChildConsole
    {
        protected:
            std::shared_ptr<Console> _console;

        public:
            ChildConsole(std::shared_ptr<Console> console);
            ~ChildConsole();
    };
} // namespace Explorer400D

#endif /* !CHILDCONSOLE_HPP_ */