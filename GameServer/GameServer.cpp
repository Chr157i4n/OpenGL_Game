#include "Platform.h"
#include "Game.h"

int main()
{
    Platform::setWorkingDirectoryToExecutable();
    Game::init();

    return 0;
}
