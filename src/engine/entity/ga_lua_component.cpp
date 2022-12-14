/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_lua_component.h"

#include "entity/ga_entity.h"
#include "framework/ga_frame_params.h"

#include <lua.hpp>
#include <iostream>

int ga_lua_component::lua_entity_translate(lua_State* state)
{
	int arg_count = lua_gettop(state);

	ga_entity* ent = (ga_entity*)lua_touserdata(state, 1);

	ga_vec3f vec;
	vec.x = (float)lua_tonumber(state, 2);
	vec.y = (float)lua_tonumber(state, 3);
	vec.z = (float)lua_tonumber(state, 4);

	ent->translate(vec);

	return 0;
}

int ga_lua_component::lua_frame_params_get_input_left(lua_State* state)
{
	int arg_count = lua_gettop(state);
	ga_frame_params* params = (ga_frame_params*)lua_touserdata(state, 1);
	lua_pushboolean(state, (params->_button_mask & k_button_j) != 0);
	return 1;
}

int ga_lua_component::lua_frame_params_get_input_right(lua_State* state)
{
	int arg_count = lua_gettop(state);
	ga_frame_params* params = (ga_frame_params*)lua_touserdata(state, 1);
	lua_pushboolean(state, (params->_button_mask & k_button_l) != 0);
	return 1;
}

ga_lua_component::ga_lua_component(ga_entity* ent, const char* path) : ga_component(ent)
{
	_lua = luaL_newstate();
	luaL_openlibs(_lua);

	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int status = luaL_dofile(_lua, fullpath.c_str());
	if (status)
	{
		std::cerr << "Failed to load script " << path << ": " << lua_tostring(_lua, -1);
		lua_close(_lua);
	}

	lua_register(_lua, "frame_params_get_input_left", lua_frame_params_get_input_left);
	lua_register(_lua, "frame_params_get_input_right", lua_frame_params_get_input_right);
	lua_register(_lua, "component_get_entity", lua_component_get_entity);
	lua_register(_lua, "entity_translate", lua_entity_translate);
}

int ga_lua_component::lua_component_get_entity(lua_State* state) {
	int arg_count = lua_gettop(state);
	ga_lua_component* component = (ga_lua_component*)lua_touserdata(state, 1);
	ga_entity* entity = component->get_entity();
	lua_pushlightuserdata(state, entity);
	return 1;
}

ga_lua_component::~ga_lua_component()
{
	lua_close(_lua);
}

void ga_lua_component::update(ga_frame_params* params)
{


	lua_getglobal(_lua, "update");
	lua_pushlightuserdata(_lua, this);
	lua_pushlightuserdata(_lua, params);

	int status = lua_pcall(_lua, 2, 0, 0);

}
