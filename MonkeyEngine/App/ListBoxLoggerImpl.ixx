module;

#include "pch.h"
#include <afxwin.h>

export module ntmonkeys.com.App.ListBoxLoggerImpl;

import ntmonkeys.com.Lib.Logger;
import <mutex>;
import <vector>;
import <string>;

export class ListBoxLoggerImpl : public Lib::Logger::Impl
{
public:
	ListBoxLoggerImpl(CListBox &listBox) noexcept;

	virtual void log(std::string message) noexcept override;

	void flush();

private:
	CListBox &__listBox;

	std::mutex __mutex;
	std::vector<std::string> __logBuffer;
};

module: private;

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