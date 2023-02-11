#include "Log.h"
#include "Path.h"

Log::Log(Path& aPath)
{
    m_logRoot = aPath.GetPolyjuiceRoot() / "logs";
    create_directory(m_logRoot);

    set_default_logger(Create("Polyjuice"));
}

Log::~Log()
{
    spdlog::default_logger()->flush();
}

std::shared_ptr<spdlog::logger> Log::Create(const TiltedPhoques::String& aName, spdlog::sink_ptr aExtraSink,
	const TiltedPhoques::String& aPattern)
{
	if (auto existingLogger = spdlog::get(aName.c_str()))
        return existingLogger;

    std::filesystem::path path = m_logRoot / (aName + ".log").c_str();

    const auto rotSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path.string(), 1048576 * 5, 3);
    rotSink->set_pattern(aPattern.c_str());
    auto logger = std::make_shared<spdlog::logger>(aName.c_str(), spdlog::sinks_init_list{ rotSink });

    if (aExtraSink)
        logger->sinks().emplace_back(aExtraSink);

#ifdef DEBUG
    logger->flush_on(spdlog::level::trace);
#else
    logger->flush_on(spdlog::level::warn);
#endif

    register_logger(logger);
    return logger;
}

