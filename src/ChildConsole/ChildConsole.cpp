#include "ChildConsole.hpp"

using namespace Explorer400D;

ChildConsole::ChildConsole(std::shared_ptr<Console> console) : _console(console)
{
}

ChildConsole::~ChildConsole()
{
}
