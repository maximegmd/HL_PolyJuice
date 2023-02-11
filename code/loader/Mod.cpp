#include "Mod.h"

std::unique_ptr<Mod> Mod::Create(std::filesystem::path aPath)
{
	spdlog::info("Loading mod \"{}\"", aPath.filename().string());

	auto m = std::make_unique<Mod>();

	std::wstring filename = aPath.filename().wstring();
	std::wstring modNameW = filename.substr(0, filename.length() - 4);
	if (modNameW.substr(modNameW.length() - 2, 2) == L"_p")
		modNameW = modNameW.substr(0, modNameW.length() - 2);
	if (modNameW.substr(modNameW.length() - 2, 2) == L"_P")
		modNameW = modNameW.substr(0, modNameW.length() - 2);
	std::string str(modNameW.begin(), modNameW.end());

	m->Name = str;

	return m;
}

void Mod::PreInit()
{
}

void Mod::PostInit()
{
}
