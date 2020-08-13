#include "Logger.h"
#include "NetworkManager.h"
#include <chrono>
#include <thread>

void sleep(int ms)
{
    std::chrono::duration<double, std::milli> delta_ms(ms);
    auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration));
}

int main()
{
    NetworkManager::init();

    bool close=false;

    while (!close)
    {
        NetworkManager::waitForEvent();

        NetworkManager::broadcastClientPosition();

        sleep(10);
    }
}