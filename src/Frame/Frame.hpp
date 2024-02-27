#ifndef FRAME_HPP_
#define FRAME_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class Frame
    {
        public:
            Frame();
            ~Frame();
            bool state = false;
            virtual void frameLoop();
            virtual void initFrame();
            virtual void saveSettings();
    };
} // namespace Explorer400D

#endif /* !FRAME_HPP_ */