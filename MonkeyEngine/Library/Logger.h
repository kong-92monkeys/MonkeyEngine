#pragma once

#include "Unique.h"
#include <memory>
#include <string>

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

		class Engine : public Unique
		{
		public:
			virtual void log(std::string message) noexcept = 0;
		};

		void emplaceEngine(std::shared_ptr<Engine> pEngine) noexcept;
		void log(const Severity severity, const std::string message) noexcept;

		[[nodiscard]]
		static Logger &getInstance() noexcept;

	private:
		std::shared_ptr<Engine> __pEngine;

		Logger() = default;

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