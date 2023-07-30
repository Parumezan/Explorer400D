#include "Console.hpp"
#include "Screen.hpp"

using namespace Explorer400D;

int main(void)
{
    std::shared_ptr<Console> console = std::make_shared<Console>(false);
    Screen screen(console);
    return 0;
}