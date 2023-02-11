#pragma once

struct Mod
{
	void PreInit();
	void PostInit();

	static std::unique_ptr<Mod> Create(std::filesystem::path aPath);

private:

	std::string Name;
};