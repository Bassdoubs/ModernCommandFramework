// PCH.h is force-included by the build (-FIPCH.h); an explicit include here would
// re-parse it after the precompiled blob and redefine Plugin::NAME/Version.
#include "Commands.h"
#include <fstream>
#include <iostream>

// Defined below (after the anonymous namespace). Forward-declare it so
// MessageCallback, which lives in the anonymous namespace above the definition,
// can call it. (Commands.cpp declares the same signature.)
void LogMessage(const std::string& message);

namespace
{
	std::ofstream g_logFile;

	void InitializeLogging()
	{
		auto path = std::filesystem::path(std::getenv("USERPROFILE")) / "Documents" / "My Games" / "Starfield" / "SFSE" / "Logs";
		std::filesystem::create_directories(path);

		path /= std::format("{}.log", Plugin::NAME);
		g_logFile.open(path.string(), std::ios::app);
	}

	void MessageCallback(SFSE::MessagingInterface::Message* a_msg) noexcept
	{
		if (!a_msg) {
			return;
		}
		// The game's ScriptCommands table is not populated during plugin load
		// (kPostLoad/kPostPostLoad), so command injection attempted there fails.
		// Retry once game data has loaded, when the table is available.
		switch (a_msg->type) {
		case SFSE::MessagingInterface::kPostDataLoad:
		case SFSE::MessagingInterface::kPostPostDataLoad:
			LogMessage(std::format("MCF: message {} received - installing registered ConsoleCommands", a_msg->type));
			Commands::OnGameDataLoaded();
			break;
		default:
			break;
		}
	}
}

void LogMessage(const std::string& message)
{
	if (g_logFile.is_open()) {
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
		
		std::tm tm;
		localtime_s(&tm, &time_t);
		
		g_logFile << std::format("[{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}] [INFO] {}\n",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec, ms.count(),
			message);
		g_logFile.flush();
	}
}

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
	SFSE::Init(a_sfse, false);
	InitializeLogging();

	LogMessage(std::format("{} v{}.1.6 loaded (hotkey-opcode-fix, ExecuteHook OFF)", Plugin::NAME, Plugin::Version));
	LogMessage("Built with libxe's updated CommonLibSF");
	LogMessage("About to call InstallHooks()...");

	Commands::InstallHooks();
	
	LogMessage("InstallHooks() returned");
	SFSE::GetMessagingInterface()->RegisterListener(MessageCallback);

	return true;
}
