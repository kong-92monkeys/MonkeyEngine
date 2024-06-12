export module ntmonkeys.com.Lib.Logger;

import ntmonkeys.com.Lib.Unique;
import <memory>;
import <vector>;
import <string>;
import <mutex>;
import <sstream>;
import <chrono>;
import <format>;

namespace Lib
{
	export class Logger : public Unique
	{
	public:
		enum class Severity
		{
			FATAL,
			WARNING,
			INFO,
			VERBOSE
		};

		class Impl : public Unique
		{
		public:
			virtual void log(std::string message) noexcept = 0;
		};

		static void emplaceImpl(std::shared_ptr<Impl> pImpl) noexcept;
		static void log(const Severity severity, const std::string message) noexcept;

	private:
		std::mutex __mutex;

		std::vector<std::string> __logBuffer;
		std::shared_ptr<Impl> __pImpl;

		Logger() = default;

		void __emplaceImpl(std::shared_ptr<Impl> &&pImpl) noexcept;
		void __log(const Severity severity, const std::string &message) noexcept;

		[[nodiscard]]
		static Logger &__getInstance() noexcept;

		[[nodiscard]]
		static std::string __makeLogMessage(const Severity severity, const std::string &message) noexcept;

		[[nodiscard]]
		static std::string __getCurrentTimeStr() noexcept;

		[[nodiscard]]
		static constexpr const char *__getSeverityStrOf(const Severity severity) noexcept;
	};
}

module: private;

namespace Lib
{
	void Logger::emplaceImpl(std::shared_ptr<Impl> pImpl) noexcept
	{
		__getInstance().__emplaceImpl(std::move(pImpl));
	}

	void Logger::log(const Severity severity, const std::string message) noexcept
	{
		__getInstance().__log(severity, message);
	}

	void Logger::__emplaceImpl(std::shared_ptr<Impl> &&pImpl) noexcept
	{
		std::lock_guard lock{ __mutex };

		__pImpl = std::move(pImpl);
		if (!__pImpl)
			return;

		for (auto &logMessage : __logBuffer)
			__pImpl->log(std::move(logMessage));

		__logBuffer.clear();
	}

	void Logger::__log(const Severity severity, const std::string &message) noexcept
	{
		auto logMessage{ __makeLogMessage(severity, message) };

		std::lock_guard lock{ __mutex };

		if (__pImpl)
			__pImpl->log(std::move(logMessage));
		else
			__logBuffer.emplace_back(std::move(logMessage));
	}

	Logger &Logger::__getInstance() noexcept
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
		return std::format("{:%y-%m-%d %H:%M:%OS}", localNow);
	}

	constexpr const char *Logger::__getSeverityStrOf(const Severity severity) noexcept
	{
		switch (severity)
		{
			case Severity::FATAL: return "FATAL";
			case Severity::WARNING: return "WARNING";
			case Severity::INFO: return "INFO";
			case Severity::VERBOSE: return "VERBOSE";
		};

		return "Unknown";
	}
}