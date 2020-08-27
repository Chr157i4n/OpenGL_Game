#include "LuaManager.h"

#include "Game.h"


lua_State* LuaManager::L;

static int lua_toggleFlashlight(lua_State* L)
{
	Game::players[0]->toggleFlashlight();

	return 0;
}

static int lua_togglePointlight(lua_State* L)
{
	int lightNumber = (int)lua_tonumber(L,1);

	glm::vec3 newLightColor = glm::vec3(0, 0, 0);

	if (!Map::pointLights[lightNumber].active)
	{
		newLightColor = Map::pointLights[lightNumber].color;
	}
	Renderer::getShader(ShaderType::basic)->bind();
	glUniform3fv(Map::pointLights[lightNumber].colorUniformIndex, 1, (float*)&newLightColor);

	Map::pointLights[lightNumber].active = !Map::pointLights[lightNumber].active;
	return 0;
}


void LuaManager::init()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	registerFunction("ToggleFlashlight", lua_toggleFlashlight);
	registerFunction("TogglePointlight", lua_togglePointlight);
}

void LuaManager::deinit()
{
	lua_close(L);
}

bool LuaManager::checkLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		Logger::log("[LUA Error]" + errormsg);
		return false;
	}
}

void LuaManager::registerFunction(std::string luafunctionIndentifierer, lua_CFunction function)
{
	lua_register(L, luafunctionIndentifierer.c_str(), function);
}

void LuaManager::loadScripts()
{
	Logger::log("Loading LUA Scripts");
	checkLua(L, luaL_dofile(L, "scripts/button1.lua"));
}

void LuaManager::runFunction(std::string lua_function)
{
	lua_getglobal(L, lua_function.c_str());
	if (lua_isfunction(L, -1))
	{
		//lua_pushnumber(L, 3.5f);

		if (checkLua(L, lua_pcall(L, 0, 1, 0)))
		{
			std::string returnValue = lua_tostring(L, -1);
			Logger::log("[C++] called in Lua: " + returnValue);
		}
	}
}


int LuaManager::lua_Hostfunction(lua_State* L)
{
	float a = (float)lua_tonumber(L, 1);
	float b = (float)lua_tonumber(L, 2);

	Logger::log("[C++] HostFunction called");
	float c = a * b;

	lua_pushnumber(L, c);

	return 1;
}

void LuaManager::testLua()
{	
	registerFunction("HostFunction", LuaManager::lua_Hostfunction);

	checkLua(L, luaL_dofile(L, "scripts/test.lua"));

	StopWatch stopwatchlua;
	stopwatchlua.start();

	lua_getglobal(L, "AddStuff");
	if (lua_isfunction(L, -1))
	{
		lua_pushnumber(L, 3.5f);
		lua_pushnumber(L, 7.1f);

		if (checkLua(L, lua_pcall(L, 2, 1, 0)))
		{
			float returnValue = (float)lua_tonumber(L, -1);
			Logger::log("[C++] called in Lua: " + std::to_string(returnValue));
		}
	}

	lua_getglobal(L, "DoAThing");
	if (lua_isfunction(L, -1))
	{
		lua_pushnumber(L, 2.0f);
		lua_pushnumber(L, 3.0f);

		if (checkLua(L, lua_pcall(L, 2, 1, 0)))
		{
			float returnValue = (float)lua_tonumber(L, -1);
			Logger::log("[C++] called in Lua: " + std::to_string(returnValue));
		}
	}

	float64 time = stopwatchlua.stop();
	Logger::log("Lua took " + std::to_string(time) + " ms");
}
