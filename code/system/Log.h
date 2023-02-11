#pragma once

struct Path;

struct Log
{
	Log(Path& aPath);
	~Log();

	std::shared_ptr<spdlog::logger> Create(const TiltedPhoques::String& aName, spdlog::sink_ptr aExtraSink = nullptr, const TiltedPhoques::String& aPattern = "[%Y-%m-%d %H:%M:%S UTC%z] [%l] %v");

private:

	std::filesystem::path m_logRoot;
};
