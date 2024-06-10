#pragma once

#include "Unique.h"
#include <memory>
#include <string_view>

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
			virtual void log(const Severity severity, const std::string_view &message) noexcept = 0;
		};

		void emplaceEngine(std::shared_ptr<Engine> pEngine) noexcept;
		void log(const Severity severity, const std::string_view &message) noexcept;

		[[nodiscard]]
		static Logger &getInstance() noexcept;

	private:
		std::shared_ptr<Engine> __pEngine;

		Logger() = default;
	};
}