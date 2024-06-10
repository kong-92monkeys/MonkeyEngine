#include "pch.h"
#include "ListBoxLoggerEngine.h"

ListBoxLoggerEngine::ListBoxLoggerEngine(CListBox &listBox) noexcept :
	__listBox{ listBox }
{}

void ListBoxLoggerEngine::log(
	const Lib::Logger::Severity severity,
	const std::string_view &message) noexcept
{
	std::lock_guard lock{ __mutex };
	__logBuffer.emplace_back(std::format("[{} {}] {}", "time", "severity", message));
}

void ListBoxLoggerEngine::flush()
{
	std::vector<std::string> logBuffer;

	{
		std::lock_guard lock{ __mutex };
		logBuffer = std::move(__logBuffer);
	}

	for (const auto &str : logBuffer)
		__listBox.AddString(CA2CT(str.c_str()));
}