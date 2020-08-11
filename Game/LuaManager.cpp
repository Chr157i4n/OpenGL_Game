#include "LuaManager.h"

#include "Game.h"


bool LuaManager::checkLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		Logger::log("[LUA Error]" + errormsg);
		return false;
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
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "HostFunction", LuaManager::lua_Hostfunction);

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


	lua_close(L);

	float64 time = stopwatchlua.stop();
	Logger::log("Lua took " + std::to_string(time) + " ms");
}
