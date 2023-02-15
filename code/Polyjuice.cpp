#include "Polyjuice.h"
#include "HogwartsSDK.h"

#include "hooks/Engine.h"

void* GetExtension(uint32_t aId)
{
	return nullptr;
}

Polyjuice::Polyjuice()
	: m_log(m_paths)
{
	Hogwarts::Initialize();

	InstallEngineHook();

	m_info.MajorVersion = 0;
	m_info.MinorVersion = 1;
	m_info.PatchVersion = 0;
	m_info.ApiVersion = 0;
	m_info.GetExtension = GetExtension;

	spdlog::info("Polyjuice loaded correctly!");
}

Polyjuice::~Polyjuice()
{
}

void Polyjuice::OnThreadAttach()
{
}

void Polyjuice::OnThreadDetach()
{
}

void Polyjuice::LoadPlugins()
{
	spdlog::info("Loading plugins...");

	m_info.FindUObject = Hogwarts::GFindUObject;
	m_info.ToString = Hogwarts::GToString;
	m_info.GUObjectArray = Hogwarts::GUObjectArray;

	auto path = m_paths.GetPolyjuiceRoot();
	for (auto path : std::filesystem::directory_iterator(path))
	{
		if (path.is_regular_file() && path.path().extension() == ".dll")
		{
			auto p = Plugin::Create(path.path(), &m_info);
			if (p)
			{
				m_plugins.emplace_back(std::move(p));
			}
		}
	}

	spdlog::info("Loaded {} plugins!", m_plugins.size());
}

void Polyjuice::LoadMods()
{
	spdlog::info("Loading mods...");

	auto path = m_paths.GetModRoot();
	for (auto path : std::filesystem::directory_iterator(path))
	{
		if (path.is_regular_file() && path.path().extension() == ".pak")
		{
			auto p = Mod::Create(path.path());
			if (p)
			{
				m_mods.emplace_back(std::move(p));
			}
		}
	}

	spdlog::info("Loaded {} mods!", m_mods.size());
}

void Polyjuice::DispatchPreInitialization()
{
	for (auto& p : m_plugins)
		p->PreInit();
}

void Polyjuice::DispatchPostInitialization()
{
	for (auto& p : m_plugins)
		p->PostInit();
}

void Polyjuice::DispatchBeginPlay(APlayerController* apController)
{
	for (auto& p : m_plugins)
		p->BeginPlay(apController);
}

void Polyjuice::DispatchTick(APlayerController* apController, float DeltaSeconds)
{
	for (auto& p : m_plugins)
		p->Tick(apController, DeltaSeconds);
}

void Polyjuice::ProcessInitGameState()
{
	for (auto& m : m_mods)
	{
		//LoadClassFromString()
	}
}
