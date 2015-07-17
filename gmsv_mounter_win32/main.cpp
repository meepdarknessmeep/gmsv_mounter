#define GMMODULE

#include "Interface.h"
#include <Windows.h>
#include "filesystem.h"
#include "LuaShared.h"

using namespace GarrysMod::Lua;

ILuaBase *LuaInterface = 0;
CreateInterfaceFn LuaSharedCreateInterface = 0;
CLuaShared *LuaShared = 0;
IFileSystem *fsystem = 0;

int Mount(lua_State *state)
{
	const char *pathid = LUA->GetString(1);
	const char *path = LUA->GetString(2);
	fsystem->AddSearchPath(path, pathid);
	return 0;
}

int Unmount(lua_State *state)
{
	const char *pathid = LUA->GetString(1);

	fsystem->RemoveSearchPaths(pathid);
	return 0;
}

int Hierarchy(lua_State *state)
{
	const char *pathid = LUA->GetString(1);
	const char *path = LUA->GetString(2);
	fsystem->CreateDirHierarchy(path, pathid);
	return 0;
}

GMOD_MODULE_OPEN()
{
	LuaSharedCreateInterface = (decltype(LuaSharedCreateInterface))GetProcAddress(GetModuleHandleA("lua_shared.dll"), "CreateInterface");
	LuaInterface = LUA;
	LuaShared = (CLuaShared *)LuaSharedCreateInterface("LUASHARED003", 0);

	CreateInterfaceFn filesystem_stdio = (decltype(LuaSharedCreateInterface))GetProcAddress(GetModuleHandleA("filesystem_stdio.dll"), "CreateInterface");

	fsystem = (decltype(fsystem))filesystem_stdio(FILESYSTEM_INTERFACE_VERSION, 0);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushCFunction(&Mount);
	LUA->SetField(-2, "MountPath");

	LUA->PushCFunction(&Hierarchy);
	LUA->SetField(-2, "MountHierarchy");

	LUA->PushCFunction(&Unmount);
	LUA->SetField(-2, "UnmountPathID");

	LUA->Pop(1);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}