#include "Logger.h"
#include <chrono>
#include <thread>

#include "NetworkManager.h"
#include "ConfigManager.h"

#include "StopWatch.h"

void sleep(int ms)
{
    std::chrono::duration<double, std::milli> delta_ms(ms);
    auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration));
}

int main()
{
    StopWatch frametimeStopWatch;

    NetworkManager::init();

    bool close=false;

    while (!close)
    {
        frametimeStopWatch.start();
        NetworkManager::waitForEvent();

        float64 frametime = frametimeStopWatch.stop();

        if (frametime < 1000 / ConfigManager::fps_limit_server && ConfigManager::fps_limit_server != 0)
        {
            sleep(1000 / ConfigManager::fps_limit_server - frametime);
        }

        
    }
}