/*! @file logging.testHelper.h
	@brief Header file for running tests for the logging.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#ifndef TEST_INCLUDE_UTILITY_DEBUG_LOGGING_LOGGING_TEST_HELPER_H
#define TEST_INCLUDE_UTILITY_DEBUG_LOGGING_LOGGING_TEST_HELPER_H

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "Utility/Debug/Logging/logger.h"

#include <spdlog/spdlog.h>

namespace Project::Testing
{
	using Project::Utility::Debug::Logging::Logger;

	static inline void ensureLoggerInitialized(const std::string_view loggerName, const std::string_view fileName,
											   const bool truncateFile = true)
	{
		static bool initialized{false};

		if (!initialized)
		{
			initialized = Logger::initialize(loggerName, fileName, truncateFile);
		}
	}

	ATTR_NODISCARD static inline std::string readLogFile(const std::string *fileName = nullptr)
	{
		const std::string defaultFileName{"logger_test_output.log"};
		// Flush spdlog to ensure all output is written before reading
		spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

		const std::ifstream file(fileName != nullptr ? *fileName : defaultFileName);
		std::ostringstream contents;
		contents << file.rdbuf();
		return contents.str();
	}
} // namespace Project::Testing

#endif
