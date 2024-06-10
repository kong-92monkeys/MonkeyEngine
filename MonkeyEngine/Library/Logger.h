#pragma once

#include "Unique.h"
#include <memory>
#include <vector>
#include <string>
#include <mutex>

namespace Lib
{
	class Logger : public Unique
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

	constexpr const char *Logger::__getSeverityStrOf(const Severity severity) noexcept
	{
		switch (severity)
		{
			case Severity::FATAL	: return "FATAL";
			case Severity::WARNING	: return "WARNING";
			case Severity::INFO		: return "INFO";
			case Severity::VERBOSE	: return "VERBOSE";
		};

		return "Unknown";
	}
}