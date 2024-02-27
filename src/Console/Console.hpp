#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include "Exception.hpp"
#include "Frame.hpp"
#include "Includes.hpp"

namespace Explorer400D
{
    class Console : public Frame
    {
        private:
            bool _isQuiet;
            ssize_t _info;
            ssize_t _warn;
            ssize_t _error;
            std::vector<std::string> _history;
            std::unique_ptr<Exception> _exception;
            void print(ConsoleType type, std::string const &msg);
            std::string getLocalTime();
            void putHistory(std::pair<std::string, std::string> const &msg);

        public:
            Console();
            ~Console();
            bool stopSequence;
            void info(std::string const &msg);
            void warn(std::string const &msg);
            void error(std::string const &msg);
            Exception throwException(std::string const &msg);
            void saveHistory(std::string settingsPath);
            std::string getHistoryCount();
            std::string getIsoTime();
            void frameLoop() override;
    };
} // namespace Explorer400D

#endif /* !CONSOLE_HPP_ */