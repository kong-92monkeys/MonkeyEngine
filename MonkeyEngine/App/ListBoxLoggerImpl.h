#pragma once

#include "../Library/Logger.h"
#include <mutex>
#include <vector>
#include <string>
#include <afxwin.h>

class ListBoxLoggerImpl : public Lib::Logger::Impl
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