#pragma once

struct Path
{
	Path();

	const std::filesystem::path GetGameRoot() const;
	const std::filesystem::path GetPolyjuiceRoot() const;
	const std::filesystem::path GetModRoot() const;

private:
	std::filesystem::path m_gameRoot;
	std::filesystem::path m_polyjuiceRoot;
	std::filesystem::path m_modRoot;
};