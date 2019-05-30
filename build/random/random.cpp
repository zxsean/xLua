#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <random>
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>

static std::unordered_map<int, std::mt19937> RandomEngines;

static void CreateEngine(int seed)
{
    RandomEngines.erase(seed);

    std::mt19937 e(seed);
    RandomEngines[seed] = e;
}

static int L_Create(lua_State *L)
{
    auto seed = static_cast<int>(luaL_checkinteger(L, 1));
    CreateEngine(seed);
    
    return 0;
}

static int L_Next(lua_State *L)
{
    auto seed = static_cast<int>(luaL_checkinteger(L, 1));
    auto minValue = static_cast<int>(luaL_checkinteger(L, 2));
    auto maxValue = static_cast<int>(luaL_checkinteger(L, 3));

    auto iEngine = RandomEngines.find(seed);
    if (iEngine == RandomEngines.end())
    {
        CreateEngine(seed);

        iEngine = RandomEngines.find(seed);
    }

    auto &engine = iEngine->second;

    auto result = engine();
    auto diff = maxValue - minValue;
    result = result % diff + minValue;

    lua_pushinteger(L, result);
    return 1;
}

LUALIB_API int luaopen_random(lua_State *L) {
    luaL_Reg libs[] = {
        { "Create",     L_Create     },
        { "Next",   L_Next },
        { NULL, NULL }
    };
    lua_newtable(L);
    luaL_setfuncs(L, libs, 0);
    return 1;
}

#ifdef __cplusplus
}
#endif