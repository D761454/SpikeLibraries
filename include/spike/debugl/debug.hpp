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

#ifndef __DEBUG_LOGGER_HPP
#define __DEBUG_LOGGER_HPP

/*
----------------------------------------------------------------------

File Summary:
	debug class - class holding a logger, allowing for messages with
	different severity to be logged to a log queue and to file.
	Message logging allows for variadic parameters formatted into the
	message.

----------------------------------------------------------------------
*/

#include <deque>
#include <string>
#include <string_view>
#include <format>
#include <source_location>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace debugl {
	enum LogType_ : uint8_t {
		LogType_normal,
		LogType_warning,
		LogType_error
	};

	enum LogFlags_ : uint8_t {
		LogFlags_showSource = 1 << 0,
		LogFlags_toFile = 1 << 1,
		LogFlags_toFileOnly = 1 << 2,
	};

	struct TextData {
		std::string text;
		std::source_location source;
		LogType_ type;

		TextData(std::string txt, LogType_ type, std::source_location location) noexcept : text(txt), type(type), source(location) {}
	};

	struct LoggerPimpl;

	class Logger {
	private:
		LoggerPimpl* _pimpl = nullptr;

		std::string _logPath = "../Logs";
		std::string _logFile;

		std::deque<TextData*> _items;
		int32_t _maxLogs = 100;

		/*returns the current time in format: [hour : minute : seconds]*/
		std::string GetTimeLog();

		/*returns the current time in format: day_month_year-hour_minute_seconds*/
		std::string GetTimeFile();

		/*output message to log file on new line*/
		void MsgToLog(std::string& msg);

		/*adds a message to the log stack with the appropriate colour, also outputs the message to the log file*/
		void MsgToStack(const std::source_location& location, std::string& msg, LogType_ type, uint8_t flags = 0);

	public:
		bool AutoScroll = true;
		bool ScrollToBottom = false;
		bool showConsole = true;

		void SetLogPath(const std::string& path);

		/*Wipe the Log*/
		void ClearLog();

		Logger();
		~Logger();

		// delete clones
		Logger(const Logger&) = delete;
		Logger(Logger&&) = delete;

		// disallow assignment
		Logger& operator=(const Logger&) = delete;
		Logger& operator=(Logger&&) = delete;

		/*return the log stack for rendering in the console*/
		std::deque<TextData*>& GetLog();

		template<typename... T>
		void Log(const std::source_location& location, LogType_ type, std::string_view fmt, T&&... args) {
			std::string result = std::vformat(fmt, std::make_format_args(args...));

			if (type != LogType_error) {
				constexpr size_t n = sizeof...(T);

				if constexpr (n > 0) {
					auto tup = std::forward_as_tuple(args...);
					auto&& last = std::get<n - 1>(tup);

					if constexpr (std::is_same_v<std::remove_cvref_t<decltype(last)>, uint8_t>) {
						MsgToStack(location, result, type, last);
						return;
					}
				}
			}

			MsgToStack(location, result, type);
		}
	};

	class Debug {
	private:
		inline static Logger _logger = debugl::Logger();

	public:
		static void SetLogPath(const std::string& path) { _logger.SetLogPath(path); }

		static void Clear() { _logger.ClearLog(); }

		/// <summary> Logs a message to the console. Optional last flag parameter below: </summary>
		/// <param name="1">Log source file and line number</param>
		/// <param name="2">Log to File aswell</param>
		/// <param name="4">Log to File Only</param>
		template<typename... T>
		struct Log {
			/// <summary> Logs a message to the console. Optional last flag parameter below: </summary>
			/// <param name="1">Log source file and line number</param>
			/// <param name="2">Log to File aswell</param>
			/// <param name="4">Log to File Only</param>
			Log(std::string_view fmt, T&&... args, const std::source_location& location = std::source_location::current()) {
				_logger.Log(location, LogType_normal, fmt, std::forward<T>(args)...);
			}
		};

		/// <summary> Logs a message to the console. Optional last flag parameter below: </summary>
		/// <param name="1">Log source file and line number</param>
		/// <param name="2">Log to File aswell</param>
		/// <param name="4">Log to File Only</param>
		template<typename... T>
		Log(std::string_view fmt, T&&... args) -> Log<T...>;

		/// <summary> Logs a warning message to the console. Optional last flag parameter below: </summary>
		/// <param name="1">Log source file and line number</param>
		/// <param name="2">Log to File aswell</param>
		/// <param name="4">Log to File Only</param>
		template<typename... T>
		struct LogWarning {
			/// <summary> Logs a warning message to the console. Optional last flag parameter below: </summary>
			/// <param name="1">Log source file and line number</param>
			/// <param name="2">Log to File aswell</param>
			/// <param name="4">Log to File Only</param>
			LogWarning(std::string_view fmt, T&&... args, const std::source_location& location = std::source_location::current()) {
				_logger.Log(location, LogType_warning, fmt, std::forward<T>(args)...);
			}
		};

		/// <summary> Logs a warning message to the console. Optional last flag parameter below: </summary>
		/// <param name="1">Log source file and line number</param>
		/// <param name="2">Log to File aswell</param>
		/// <param name="4">Log to File Only</param>
		template<typename... T>
		LogWarning(std::string_view fmt, T&&... args) -> LogWarning<T...>;

		/// <summary> Logs an error message to the console (displays source file and line number) </summary>
		template<typename... T>
		struct LogError {
			/// <summary> Logs an error message to the console (displays source file and line number) </summary>
			LogError(std::string_view fmt, T&&... args, const std::source_location& location = std::source_location::current()) {
				_logger.Log(location, LogType_error, fmt, std::forward<T>(args)...);
			}
		};

		/// <summary> Logs an error message to the console (displays source file and line number) </summary>
		template<typename... T>
		LogError(std::string_view fmt, T&&... args) -> LogError<T...>;

		/// <summary>
		/// return the log queue for rendering in the console
		/// </summary>
		/// <returns>reference to the debug log queue</returns>
		static std::deque<TextData*>& GetLog() {
			return _logger.GetLog();
		}
	};
}

using Debug = debugl::Debug;

#endif