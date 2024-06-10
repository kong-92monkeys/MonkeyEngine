#include "Logger.h"
#include <sstream>
#include <chrono>

namespace Lib
{
	void Logger::emplaceImpl(std::shared_ptr<Impl> pImpl) noexcept
	{
		std::lock_guard lock{ __mutex };

		__pImpl = std::move(pImpl);
		if (!__pImpl)
			return;

		for (auto &logMessage : __logBuffer)
			__pImpl->log(std::move(logMessage));

		__logBuffer.clear();
	}

	void Logger::log(const Severity severity, const std::string message) noexcept
	{
		auto logMessage{ __makeLogMessage(severity, message) };

		std::lock_guard lock{ __mutex };

		if (__pImpl)
			__pImpl->log(std::move(logMessage));
		else
			__logBuffer.emplace_back(std::move(logMessage));
	}

	Logger &Logger::getInstance() noexcept
	{
		static Logger instance;
		return instance;
	}

	std::string Logger::__makeLogMessage(const Severity severity, const std::string &message) noexcept
	{
		std::ostringstream oss;
		oss << std::format("[{}][{}] {}", __getCurrentTimeStr(), __getSeverityStrOf(severity), message);

		return oss.str();
	}

	std::string Logger::__getCurrentTimeStr() noexcept
	{
		using namespace std::chrono;

		const auto localNow{ current_zone()->to_local(system_clock::now()) };
		return std::format("{:%Y-%m-%d %H:%M:%OS}", localNow);
	}
}