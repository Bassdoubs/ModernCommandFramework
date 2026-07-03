#include "MCF_API.h"

namespace Commands
{
	void InstallHooks();
	void RegisterCommand(const char* name, MCF::CommandCallback func);
	// Called once game data has finished loading (SFSE kPostDataLoad), when the
	// game's ScriptCommands table is populated and safe to inject into.
	void OnGameDataLoaded();
}