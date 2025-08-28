#pragma once

#ifdef CFSPEXP
#define CFSP_API __declspec(dllexport)
#else
#define CFSP_API __declspec(dllimport)
#endif

#define LUAAPI(name) int lua_api_##name(lua_State* L)

#define LUA_ARG_COUNT_CHECK_C(i)                                                                                       \
    if (lua_gettop(L) != (i)) return luaL_error(L, "%d args expected", (i));

#define LUA_ARG_COUNT_CHECK_M(i)                                                                                       \
    if (lua_gettop(L) != (i) + 1) return luaL_error(L, "%d args expected (without \"self\")", (i));
