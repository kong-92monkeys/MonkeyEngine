#include "Logger.h"
#include <sstream>
#include <chrono>

namespace Lib
{
	void Logger::emplaceEngine(std::shared_ptr<Engine> pEngine) noexcept
	{
		__pEngine = std::move(pEngine);
	}

	void Logger::log(const Severity severity, const std::string message) noexcept
	{

		std::ostringstream oss;
		oss << std::format("[{}][{}] {}", __getCurrentTimeStr(), __getSeverityStrOf(severity), message);

		__pEngine->log(oss.str());
	}

	Logger &Logger::getInstance() noexcept
	{
		static Logger instance;
		return instance;
	}

	std::string Logger::__getCurrentTimeStr() noexcept
	{
		using namespace std::chrono;

		const auto localNow{ current_zone()->to_local(system_clock::now()) };
		return std::format("{:%Y-%m-%d %H:%M:%OS}", localNow);
	}
}