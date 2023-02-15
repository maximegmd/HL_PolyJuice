#pragma once
#include "system/Log.h"
#include "system/Path.h"
#include "loader/Plugin.h"
#include "loader/Mod.h"

struct Polyjuice
{
	static Polyjuice& Get();

	void OnThreadAttach();
	void OnThreadDetach();

	void LoadPlugins();
	void LoadMods();
	void DispatchPreInitialization();
	void DispatchPostInitialization();
	void DispatchBeginPlay(class APlayerController* apController);
	void DispatchEndPlay(class APlayerController* apController);
	void DispatchTick(class APlayerController* apController, float DeltaSeconds);

	void ProcessInitGameState();

private:

	Polyjuice();
	~Polyjuice();

	friend struct TiltedPhoques::Allocator;

	Path m_paths;
	Log m_log;
	Plugin::PolyjuiceInfo m_info;

	TiltedPhoques::Vector<std::unique_ptr<Plugin>> m_plugins;
	TiltedPhoques::Vector<std::unique_ptr<Mod>> m_mods;
};
