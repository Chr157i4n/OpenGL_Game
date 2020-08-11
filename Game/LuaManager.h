#pragma once

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class LuaManager
{
public:
	static bool checkLua(lua_State* L, int r);

	static int lua_Hostfunction(lua_State* L);
	
	static void testLua();

};

