/*
----------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2026, Cayden Andrews

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#include "spike/debugl/debug.hpp"

#include <fstream>
#include <filesystem>
#include <ctime>
#include <cstdint>
#include <deque>
#include <source_location>
#include <string>
#include <utility>

#ifdef _DEBUG
#include <iostream>
#endif

namespace debugl {

struct LoggerPimpl {
	std::fstream log;
	time_t timestamp = time(NULL);
};

std::string Logger::GetTimeLog() {
	if (!_pimpl) return "NULL";
	time(&_pimpl->timestamp);
	struct tm datetime;

	
#if defined(_WIN32)
	localtime_s(&datetime, &_pimpl->timestamp);
#elif defined(__linux__)
	localtime_r(&_pimpl->timestamp, &datetime);
#endif

	int sec = datetime.tm_sec;

	std::string time =
		"[" +
		std::to_string(datetime.tm_hour) + ":" +
		std::to_string(datetime.tm_min) + ":" +
		(sec < 10 ? "0" : "") + std::to_string(sec) + "] ";

	return time;
}

std::string Logger::GetTimeFile() {
	if (!_pimpl) return "NULL";
	time(&_pimpl->timestamp);
	struct tm datetime;

#if defined(_WIN32)
	localtime_s(&datetime, &_pimpl->timestamp);
#elif defined(__linux__)
	localtime_r(&_pimpl->timestamp, &datetime);
#endif

	int sec = datetime.tm_sec;

	std::string time =
		std::to_string(datetime.tm_mday) + "_" +
		std::to_string(datetime.tm_mon + 1) + "_" +
		std::to_string(datetime.tm_year + 1900) + "-" +
		std::to_string(datetime.tm_hour) + "_" +
		std::to_string(datetime.tm_min) + "_" +
		(sec < 10 ? "0" : "") + std::to_string(datetime.tm_sec);

	return time;
}

void Logger::MsgToLog(std::string& msg) {
	if (!_pimpl) return;
	std::filesystem::create_directories(_logPath);

	_pimpl->log.open(_logPath + _logFile, std::fstream::app);

	if (_pimpl->log.is_open()) {
		_pimpl->log << msg << "\n";
	}

	_pimpl->log.close();
}

void Logger::MsgToStack(const std::source_location& location, std::string& msg, LogType_ type, uint8_t flags) {
	std::string result = GetTimeLog() + (flags & LogFlags_showSource ? std::filesystem::path(location.file_name()).filename().string() + "(" + std::to_string(location.line()) + "): " + msg : msg);

	if (!(flags & LogFlags_toFileOnly)) {
		TextData* td = new TextData(result, type, location);

		_items.push_back(std::move(td));

		if (_items.size() > _maxLogs) {
			_items.pop_front();
		}

		td = nullptr;
	}

	size_t pos = result.find_first_of("]") + 1;
	std::string add = type == LogType_normal ? " Log:" : type == LogType_warning ? " Warning:" : " Error:";
	result.insert(pos, add);

#ifdef _DEBUG
	std::cout << result << std::endl;
#endif

	MsgToLog(result);
}

void Logger::SetLogPath(const std::string& path) {
	_logPath = path;
}

void Logger::ClearLog() {
	for (int32_t i = 0; i < _items.size(); i++) {
		delete _items.at(i);
	}
	_items.clear();
}

Logger::Logger() {
	ClearLog();
	_pimpl = new LoggerPimpl();
	_logFile = std::string("/") + GetTimeFile() + std::string("_DebugLog.log");
}

Logger::~Logger() {
	ClearLog();
	if (_pimpl) delete _pimpl;
	_pimpl = nullptr;
}

std::deque<TextData*>& Logger::GetLog() {
	return _items;
}

}