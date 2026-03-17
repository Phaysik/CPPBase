/*! @file logger.test.cpp
	@brief Google Test unit tests for the static Logger wrapper.
	@details Exercises initialization, level get/set, name/file replacement, and all seven logging-level template methods. Each test writes
   to a temporary file that is cleaned up in TearDown.
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#include "Utility/Debug/Logging/logger.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "Core/attributeMacros.h"

#include <gtest/gtest.h>

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

using Project::Utility::Debug::Logging::Logger;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace // NOSONAR
{
	class LoggerTest : public ::testing::Test // NOSONAR
	{
		protected:
			void SetUp() override
			{
				spdlog::drop_all();

				// Remove any leftover log file from a previous test run
				static_cast<void>(std::filesystem::remove(logFileName));

				Logger::initialize(loggerName, logFileName);
			}

			void TearDown() override
			{
				spdlog::drop_all();
				static_cast<void>(std::filesystem::remove(logFileName));
			}

			/*! @brief Gets the logger name used for tests.
				@return The logger name string.
			*/
			ATTR_NODISCARD const std::string &getLoggerName() const
			{
				return loggerName;
			}

			/*! @brief Gets the log file name used for tests.
				@return The log file name string.
			*/
			ATTR_NODISCARD const std::string &getLogFileName() const
			{
				return logFileName;
			}

			/*! @brief Reads the full contents of the current log file.
				@details Flushes the spdlog logger before reading to ensure all buffered output is written.
				@return The file contents as a string.
			*/
			ATTR_NODISCARD std::string readLogFile() const
			{
				// Flush spdlog to ensure all output is written before reading
				spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

				std::ifstream file(logFileName);
				std::ostringstream contents;
				contents << file.rdbuf();
				return contents.str();
			}

		private:
			std::string loggerName{"test_logger"};
			std::string logFileName{"logger_test_output.log"};
	};

	// ────────────────────────────────────────────────────────────
	// Initialization
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenValidArgumentsWhenInitializedThenLoggerIsUsable)
	{
		// Logger was initialized in SetUp; logging should not throw
		EXPECT_NO_THROW(Logger::info("initialization smoke test"));
	}

	TEST_F(LoggerTest, GivenAlreadyInitializedWhenReInitializedThenSucceeds)
	{
		std::string secondFile = "logger_test_reinit.log";
		EXPECT_NO_THROW(Logger::initialize("reinit_logger", secondFile));

		Logger::info("after reinit");
		// The new logger writes to secondFile, not logFileName
		// Clean up the second file
		spdlog::drop_all();
		static_cast<void>(std::filesystem::remove(secondFile));
	}

	TEST_F(LoggerTest, GivenSameNameWhenReInitializedThenSucceeds)
	{
		// Re-initializing with the same name should succeed because initialize drops the old registry entry.
		EXPECT_NO_THROW(Logger::initialize(getLoggerName(), getLogFileName()));
		EXPECT_NO_THROW(Logger::info("after same-name reinit"));
	}

	// ────────────────────────────────────────────────────────────
	// getLevel / setLevel
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenDefaultLevelWhenGetLevelCalledThenReturnsInfoLevel)
	{
		// spdlog's default level is info
		spdlog::level::level_enum level = Logger::getLevel();
		EXPECT_EQ(level, spdlog::level::info);
	}

	TEST_F(LoggerTest, GivenDebugLevelWhenSetLevelCalledThenGetLevelReturnsDebug)
	{
		Logger::setLevel(spdlog::level::debug);
		EXPECT_EQ(Logger::getLevel(), spdlog::level::debug);
	}

	TEST_F(LoggerTest, GivenTraceLevelWhenSetLevelCalledThenGetLevelReturnsTrace)
	{
		Logger::setLevel(spdlog::level::trace);
		EXPECT_EQ(Logger::getLevel(), spdlog::level::trace);
	}

	TEST_F(LoggerTest, GivenCriticalLevelWhenSetLevelCalledThenGetLevelReturnsCritical)
	{
		Logger::setLevel(spdlog::level::critical);
		EXPECT_EQ(Logger::getLevel(), spdlog::level::critical);
	}

	// ────────────────────────────────────────────────────────────
	// setLoggerName
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenNewNameWhenSetLoggerNameCalledThenLoggerStillWorks)
	{
		EXPECT_NO_THROW(Logger::setLoggerName("renamed_logger"));
		EXPECT_NO_THROW(Logger::info("after rename"));
	}

	// ────────────────────────────────────────────────────────────
	// setFileName
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenNewFileWhenSetFileNameCalledThenLogsToNewFile)
	{
		std::string newFile = "logger_test_newfile.log";
		static_cast<void>(std::filesystem::remove(newFile));

		Logger::setFileName(newFile);
		Logger::info("written to new file");

		// Flush and verify output landed in the new file
		spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

		std::ifstream file(newFile);
		std::ostringstream contents;
		contents << file.rdbuf();
		EXPECT_NE(contents.str().find("written to new file"), std::string::npos);

		spdlog::drop_all();
		static_cast<void>(std::filesystem::remove(newFile));
	}

	// ────────────────────────────────────────────────────────────
	// setLoggerAndFileName
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenNewNameAndFileWhenSetLoggerAndFileNameCalledThenLogsToNewFile)
	{
		std::string newFile = "logger_test_both.log";
		static_cast<void>(std::filesystem::remove(newFile));

		Logger::setLoggerAndFileName("both_logger", newFile);
		Logger::info("both changed");

		spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

		std::ifstream file(newFile);
		std::ostringstream contents;
		contents << file.rdbuf();
		EXPECT_NE(contents.str().find("both changed"), std::string::npos);

		spdlog::drop_all();
		static_cast<void>(std::filesystem::remove(newFile));
	}

	// ────────────────────────────────────────────────────────────
	// Template logging methods — verify output is written
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenTraceLevelEnabledWhenTraceCalledThenMessageAppearsInLog)
	{
		Logger::setLevel(spdlog::level::trace);
		Logger::trace("trace message {}", 42);

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("trace message 42"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenDebugLevelEnabledWhenDebugCalledThenMessageAppearsInLog)
	{
		Logger::setLevel(spdlog::level::debug);
		Logger::debug("debug message {}", "hello");

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("debug message hello"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenInfoCalledThenMessageAppearsInLog)
	{
		Logger::info("info message {}", 100);

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("info message 100"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledThenMessageAppearsInLog)
	{
		Logger::warn("warn message {}", 3.14);

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("warn message 3.14"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenErrorCalledThenMessageAppearsInLog)
	{
		Logger::error("error message {}", "failure");

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("error message failure"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenCriticalCalledThenMessageAppearsInLog)
	{
		Logger::critical("critical message {}", 999);

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("critical message 999"), std::string::npos);
	}

	// ────────────────────────────────────────────────────────────
	// Level filtering — messages below threshold are suppressed
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenErrorLevelWhenInfoCalledThenMessageIsNotWritten)
	{
		Logger::setLevel(spdlog::level::err);
		Logger::info("should not appear");

		const std::string contents{readLogFile()};
		EXPECT_EQ(contents.find("should not appear"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenCriticalLevelWhenWarnCalledThenMessageIsNotWritten)
	{
		Logger::setLevel(spdlog::level::critical);
		Logger::warn("suppressed warning");

		const std::string contents{readLogFile()};
		EXPECT_EQ(contents.find("suppressed warning"), std::string::npos);
	}

	// ────────────────────────────────────────────────────────────
	// Multiple messages accumulate in the same log file
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenMultipleInfoCallsWhenReadThenAllMessagesPresent)
	{
		Logger::info("first message");
		Logger::info("second message");
		Logger::info("third message");

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("first message"), std::string::npos);
		EXPECT_NE(contents.find("second message"), std::string::npos);
		EXPECT_NE(contents.find("third message"), std::string::npos);
	}

	// ────────────────────────────────────────────────────────────
	// Format string with no arguments
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenPlainStringWhenInfoCalledThenMessageAppearsInLog)
	{
		Logger::info("no format args");

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("no format args"), std::string::npos);
	}
} // namespace

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
