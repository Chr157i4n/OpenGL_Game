- lua script files can have any name
- functions need to have different name globally

- there should be one lua file per map containing all needed functions
- functions should have a prefix of the map

Functions available in lua:


void toggleFlashlight()

void togglePointlight(int id)

float, float, float getObjectPosition(int id)

void setObjectPosition(float x,float y, float z)

float getObjectHealth(int id)

void addToObjectHealth(int id, float health)

float getGameDelta()

float getGameTimeStamp()