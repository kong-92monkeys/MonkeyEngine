#include "Logger.h"

namespace Lib
{
	void Logger::emplaceEngine(std::shared_ptr<Engine> pEngine) noexcept
	{
		__pEngine = std::move(pEngine);
	}

	void Logger::log(const Severity severity, const std::string_view &message) noexcept
	{
		__pEngine->log(severity, message);
	}

	Logger &Logger::getInstance() noexcept
	{
		static Logger instance;
		return instance;
	}
}