#include "Plugin.h"

void Plugin::PreInit()
{
	if (m_info.OnPreInitialize)
		(m_info.OnPreInitialize)();
}

void Plugin::PostInit()
{
	if (m_info.OnPostInitialize)
		(m_info.OnPostInitialize)();
}

void Plugin::BeginPlay(APlayerController* apController)
{
	if (m_info.OnBeginPlay)
		(m_info.OnBeginPlay)(apController);
}

void Plugin::Tick(APlayerController* apController, float DeltaSeconds)
{
	if (m_info.OnTick)
		(m_info.OnTick)(apController, DeltaSeconds);
}

std::unique_ptr<Plugin> Plugin::Create(std::filesystem::path aPath, PolyjuiceInfo* apInfo)
{
	spdlog::info("Loading plugin \"{}\"", aPath.filename().string());

	auto handle = LoadLibrary(aPath.c_str());
	if (!handle)
	{
		spdlog::error("Failed to load plugin \"{}\"", aPath.filename().string());
		return {};
	}

	auto proc = GetProcAddress(handle, "PolyjuiceQuery");
	if (!proc)
	{
		spdlog::error("Failed to load plugin \"{}\", missing PolyjuiceQuery export", aPath.filename().string());
		FreeLibrary(handle);
		return {};
	}

	auto p = std::make_unique<Plugin>();
	p->QueryCallback = (Plugin::TQuery)proc;
	auto result = (p->QueryCallback)(apInfo, &p->m_info);
	if (!result)
	{
		spdlog::error("Failed to load plugin \"{}\", PolyjuiceQuery returned false", aPath.filename().string());
		FreeLibrary(handle);
		return {};
	}

	auto* info = p->GetInfo();
	spdlog::info("Loaded plugin \"{}\", version {}.{}.{}!", info->Name, info->MajorVersion, info->MinorVersion, info->PatchVersion);

	return p;
}
