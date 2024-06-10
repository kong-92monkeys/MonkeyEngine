#pragma once

#include "../Library/Logger.h"
#include <mutex>
#include <vector>
#include <string>
#include <afxwin.h>

class ListBoxLoggerEngine : public Lib::Logger::Engine
{
public:
	ListBoxLoggerEngine(CListBox &listBox) noexcept;

	virtual void log(
		const Lib::Logger::Severity severity,
		const std::string_view &message) noexcept override;

	void flush();

private:
	CListBox &__listBox;

	std::mutex __mutex;
	std::vector<std::string> __logBuffer;
};