#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class Screen
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            void windowLoop();
            void mainMenuBar();

        public:
            Screen();
            ~Screen();
    };
} // namespace Explorer400D

#endif /* !SCREEN_HPP_ */