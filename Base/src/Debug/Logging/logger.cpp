/*! \file logger.cpp
	\brief Contains the function definitions for creating a logger
	\date 03/11/2026
	\version x.x.x
	\since x.x.x
	\author Matthew Moore
*/

#include "Utility/Debug/Logging/logger.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "attributeMacros.h"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace Utility::Debug::Logging
{
	// MARK: Getter

	ATTR_NODISCARD spdlog::level::level_enum Logger::getLevel()
	{
		return getLoggerInstance()->level();
	}

	// MARK: Setters

	void Logger::setLevel(spdlog::level::level_enum level)
	{
		getLoggerInstance()->set_level(level);
	}

	void Logger::setLoggerName(const std::string &loggerName)
	{
		initialize(loggerName, getFileNameStore());
	}

	void Logger::setFileName(const std::string &fileName)
	{
		initialize(getLoggerName(), fileName);
	}

	void Logger::setLoggerAndFileName(const std::string &loggerName, const std::string &fileName)
	{
		initialize(loggerName, fileName);
	}

	// MARK: Static Member Function

	void Logger::initialize(std::string_view loggerName, std::string_view fileName)
	{
		const std::string convertedLoggerName{loggerName};
		const std::string convertedFileName{fileName};

		if (getLoggerInstance())
		{
			spdlog::drop(getLoggerName());
		}

		getLoggerInstance().reset();

		getLoggerName() = convertedLoggerName;
		getFileNameStore() = convertedFileName;

		try
		{
			getLoggerInstance() = spdlog::basic_logger_mt(convertedLoggerName, convertedFileName);
		}
		catch (const spdlog::spdlog_ex &ex)
		{
			throw std::runtime_error(std::string("Log initialization failed: ") + ex.what());
		}
	}

// GCC incorrectly suggests returns_nonnull for reference-returning functions; suppress since references are inherently non-null.
#if defined(__GNUC__) && !defined(__clang__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsuggest-attribute=returns_nonnull"
#endif

	std::shared_ptr<spdlog::logger> &Logger::getLoggerInstance()
	{
		static std::shared_ptr<spdlog::logger> logger;
		return logger;
	}

	std::string &Logger::getLoggerName()
	{
		static std::string loggerName;
		return loggerName;
	}

	std::string &Logger::getFileNameStore()
	{
		static std::string fileName;
		return fileName;
	}

#if defined(__GNUC__) && !defined(__clang__)
	#pragma GCC diagnostic pop
#endif
} // namespace Utility::Debug::Logging