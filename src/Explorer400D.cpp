#include "Screen/Screen.hpp"

using namespace Explorer400D;

bool stopSignal = false;

void signalHandler(int signum)
{
    spdlog::warn("Interrupt signal ({}) received", signum);
    stopSignal = true;
}

int main(void)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Screen screen;
    return 0;
}