#include "Path.h"

Path::Path()
{
	char path[4096];

	auto size = GetModuleFileNameA(GetModuleHandleA(nullptr), path, std::size(path));
	std::filesystem::path exePath = path;

	m_gameRoot = exePath.parent_path() / ".." / ".." / "..";
	m_polyjuiceRoot = m_gameRoot / "Polyjuice";
	m_modRoot = m_gameRoot / "Phoenix" / "Content" / "Paks" / "Mods";

	create_directory(m_polyjuiceRoot);
	create_directory(m_modRoot);
}

const std::filesystem::path Path::GetGameRoot() const
{
	return m_gameRoot;
}

const std::filesystem::path Path::GetPolyjuiceRoot() const
{
	return m_polyjuiceRoot;
}

const std::filesystem::path Path::GetModRoot() const
{
	return m_modRoot;
}

