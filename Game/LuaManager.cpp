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
	//checkLua(L, luaL_dofile(L, "scripts/button1.lua"));
	std::vector<std::string> scripts = readAllScripts();

	for (std::string script : scripts)
	{
		checkLua(L, luaL_dofile(L, script.c_str()));
	}
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

std::vector<std::string> LuaManager::readAllScripts()
{
	std::vector<std::string> scripts;
	std::string path = "scripts";
	if (!std::filesystem::exists(path)) return scripts;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.path().string().find(".lua") != std::string::npos) {
			scripts.push_back(entry.path().string());
		}
		
	}

	return scripts;
}