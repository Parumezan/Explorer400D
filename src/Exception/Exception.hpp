#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include "Includes.hpp"

namespace Explorer400D
{
    class Exception : public std::exception
    {
        private:
            std::string _msg;

        public:
            Exception(std::string const &msg);
            ~Exception() throw();
            const char *what() const throw();
    };
} // namespace Explorer400D

#endif /* !EXCEPTION_HPP_ */