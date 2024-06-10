#include "pch.h"
#include "ListBoxLoggerImpl.h"

ListBoxLoggerImpl::ListBoxLoggerImpl(CListBox &listBox) noexcept :
	__listBox{ listBox }
{}

void ListBoxLoggerImpl::log(std::string message) noexcept
{
	std::lock_guard lock{ __mutex };
	__logBuffer.emplace_back(std::move(message));
}

void ListBoxLoggerImpl::flush()
{
	std::vector<std::string> logBuffer;

	{
		std::lock_guard lock{ __mutex };
		logBuffer = std::move(__logBuffer);
	}

	for (const auto &str : logBuffer)
		__listBox.AddString(CA2CT(str.c_str()));
}