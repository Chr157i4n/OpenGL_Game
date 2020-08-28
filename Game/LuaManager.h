#pragma once

#include <functional>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class LuaManager
{
public:

	static void init();

	static void deinit();

	static bool checkLua(lua_State* L, int r);

	static void registerFunction(std::string luafunctionIndentifierer, lua_CFunction function);

	static void loadScripts();

	static void runFunction(std::string lua_function);


private:
	static std::vector<std::string> readAllScripts();

	static lua_State* L;

};

