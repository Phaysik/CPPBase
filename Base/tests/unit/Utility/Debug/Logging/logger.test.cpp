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
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Core/attributeMacros.h"
#include "Utility/Debug/Logging/constants.h"

#include <gtest/gtest.h>

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
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

				static_cast<void>(Logger::initialize(loggerName, logFileName));
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
		// Logger was initialized in SetUp; logging should succeed
		std::optional<std::string_view> result{Logger::info("initialization smoke test")};
		EXPECT_FALSE(result.has_value());
	}

	TEST_F(LoggerTest, GivenAlreadyInitializedWhenReInitializedThenSucceeds)
	{
		std::string secondFile{"logger_test_reinit.log"};
		bool initResult{Logger::initialize("reinit_logger", secondFile)};
		EXPECT_TRUE(initResult);

		std::optional<std::string_view> result{Logger::info("after reinit")};
		EXPECT_FALSE(result.has_value());
		// The new logger writes to secondFile, not logFileName
		// Clean up the second file
		spdlog::drop_all();
		static_cast<void>(std::filesystem::remove(secondFile));
	}

	TEST_F(LoggerTest, GivenSameNameWhenReInitializedThenSucceeds)
	{
		// Re-initializing with the same name should succeed because initialize drops the old registry entry.
		bool initResult{Logger::initialize(getLoggerName(), getLogFileName())};
		EXPECT_TRUE(initResult);

		std::optional<std::string_view> result{Logger::info("after same-name reinit")};
		EXPECT_FALSE(result.has_value());
	}

	// ────────────────────────────────────────────────────────────
	// getLevel / setLevel
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenDefaultLevelWhenGetLevelCalledThenReturnsInfoLevel)
	{
		// spdlog's default level is info
		spdlog::level::level_enum level{Logger::getLevel()};
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
		const bool setResult{Logger::setLoggerName("renamed_logger")};
		EXPECT_TRUE(setResult);

		std::optional<std::string_view> result{Logger::info("after rename")};
		EXPECT_FALSE(result.has_value());
	}

	// ────────────────────────────────────────────────────────────
	// setFileName
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenNewFileWhenSetFileNameCalledThenLogsToNewFile)
	{
		std::string newFile{"logger_test_newfile.log"};
		static_cast<void>(std::filesystem::remove(newFile));

		const bool setResult{Logger::setFileName(newFile)};
		EXPECT_TRUE(setResult);

		std::optional<std::string_view> result{Logger::info("written to new file")};
		EXPECT_FALSE(result.has_value());

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
		std::string newFile{"logger_test_both.log"};
		static_cast<void>(std::filesystem::remove(newFile));

		const bool setResult{Logger::setLoggerAndFileName("both_logger", newFile)};
		EXPECT_TRUE(setResult);

		std::optional<std::string_view> result{Logger::info("both changed")};
		EXPECT_FALSE(result.has_value());

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

	TEST_F(LoggerTest, GivenInfoLevelWhenLogCalledThenMessageAppearsInLog)
	{
		std::optional<std::string_view> result{Logger::log(spdlog::level::info, "log message {}", 77)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("log message 77"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenTraceLevelEnabledWhenTraceCalledThenMessageAppearsInLog)
	{
		Logger::setLevel(spdlog::level::trace);
		std::optional<std::string_view> result{Logger::trace("trace message {}", 42)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("trace message 42"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenDebugLevelEnabledWhenDebugCalledThenMessageAppearsInLog)
	{
		Logger::setLevel(spdlog::level::debug);
		std::optional<std::string_view> result{Logger::debug("debug message {}", "hello")};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("debug message hello"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenInfoCalledThenMessageAppearsInLog)
	{
		std::optional<std::string_view> result{Logger::info("info message {}", 100)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("info message 100"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledThenMessageAppearsInLog)
	{
		std::optional<std::string_view> result{Logger::warn("warn message {}", 3.14)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("warn message 3.14"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenErrorCalledThenMessageAppearsInLog)
	{
		std::optional<std::string_view> result{Logger::error("error message {}", "failure")};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("error message failure"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenCriticalCalledThenMessageAppearsInLog)
	{
		std::optional<std::string_view> result{Logger::critical("critical message {}", 999)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("critical message 999"), std::string::npos);
	}

	// ────────────────────────────────────────────────────────────
	// Level filtering — messages below threshold are suppressed
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenErrorLevelWhenInfoCalledThenMessageIsNotWritten)
	{
		Logger::setLevel(spdlog::level::err);
		std::optional<std::string_view> result{Logger::info("should not appear")};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_EQ(contents.find("should not appear"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenCriticalLevelWhenWarnCalledThenMessageIsNotWritten)
	{
		Logger::setLevel(spdlog::level::critical);
		std::optional<std::string_view> result{Logger::warn("suppressed warning")};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_EQ(contents.find("suppressed warning"), std::string::npos);
	}

	// ────────────────────────────────────────────────────────────
	// Multiple messages accumulate in the same log file
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenMultipleInfoCallsWhenReadThenAllMessagesPresent)
	{
		auto result1{Logger::info("first message")};
		auto result2{Logger::info("second message")};
		auto result3{Logger::info("third message")};
		EXPECT_FALSE(result1.has_value());
		EXPECT_FALSE(result2.has_value());
		EXPECT_FALSE(result3.has_value());

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
		std::optional<std::string_view> result{Logger::info("no format args")};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("no format args"), std::string::npos);
	}

	// ────────────────────────────────────────────────────────────
	// Template instantiations matching configuration.cpp call sites
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenInfoLevelWhenInfoCalledWithStringViewArgThenMessageAppearsInLog)
	{
		const std::string_view arg{"type_name"};
		std::optional<std::string_view> result{Logger::info("registered {}", arg)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("registered type_name"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenInfoCalledWithTwoStringViewArgsThenMessageAppearsInLog)
	{
		const std::string_view arg1{"fire"};
		const std::string_view arg2{"water"};
		std::optional<std::string_view> result{Logger::info("{} vs {}", arg1, arg2)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("fire vs water"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenInfoCalledWithUnsignedCharArgThenMessageAppearsInLog)
	{
		const unsigned char arg{5};
		std::optional<std::string_view> result{Logger::info("type id {}", arg)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("type id 5"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledWithStringViewArgThenMessageAppearsInLog)
	{
		const std::string_view arg{"warning_detail"};
		std::optional<std::string_view> result{Logger::warn("issue: {}", arg)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("issue: warning_detail"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledWithTwoStringViewArgsThenMessageAppearsInLog)
	{
		const std::string_view arg1{"expected"};
		const std::string_view arg2{"actual"};
		std::optional<std::string_view> result{Logger::warn("{} != {}", arg1, arg2)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("expected != actual"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledWithUnsignedCharAndStringViewArgsThenMessageAppearsInLog)
	{
		const unsigned char userID{3};
		const std::string_view name{"grass"};
		std::optional<std::string_view> result{Logger::warn("id {} name {}", userID, name)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("id 3 name grass"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledWithTwoUnsignedCharArgsThenMessageAppearsInLog)
	{
		const unsigned char id1{1};
		const unsigned char id2{2};
		std::optional<std::string_view> result{Logger::warn("ids {} {}", id1, id2)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("ids 1 2"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInfoLevelWhenWarnCalledWithUnsignedLongAndUnsignedCharArgsThenMessageAppearsInLog)
	{
		const unsigned char userID{7};
		std::optional<std::string_view> result{Logger::warn("size {} id {}", 20UL, userID)};
		EXPECT_FALSE(result.has_value());

		const std::string contents{readLogFile()};
		EXPECT_NE(contents.find("size 20 id 7"), std::string::npos);
	}

	// Failure coverage for configuration.cpp instantiations

	TEST_F(LoggerTest, GivenSpdlogExWhenInfoCalledWithStringViewArgThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const std::string_view arg{"x"};
		std::optional<std::string_view> result{Logger::info("{} {}", arg)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenInfoCalledWithTwoStringViewArgsThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const std::string_view arg1{"a"};
		const std::string_view arg2{"b"};
		std::optional<std::string_view> result{Logger::info("{} {} {}", arg1, arg2)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenInfoCalledWithUnsignedCharArgThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const unsigned char arg{1};
		std::optional<std::string_view> result{Logger::info("{} {}", arg)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenWarnCalledWithStringViewArgThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const std::string_view arg{"x"};
		std::optional<std::string_view> result{Logger::warn("{} {}", arg)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenWarnCalledWithTwoStringViewArgsThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const std::string_view arg1{"a"};
		const std::string_view arg2{"b"};
		std::optional<std::string_view> result{Logger::warn("{} {} {}", arg1, arg2)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenWarnCalledWithUnsignedCharAndStringViewArgsThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const unsigned char userID{1};
		const std::string_view name{"x"};
		std::optional<std::string_view> result{Logger::warn("{} {} {}", userID, name)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenWarnCalledWithTwoUnsignedCharArgsThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const unsigned char id1{1};
		const unsigned char id2{2};
		std::optional<std::string_view> result{Logger::warn("{} {} {}", id1, id2)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenWarnCalledWithUnsignedLongAndUnsignedCharArgsThenReturnsFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });
		const unsigned char userID{1};
		std::optional<std::string_view> result{Logger::warn("{} {} {}", 20UL, userID)};
		ASSERT_TRUE(result.has_value());
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenInfoCalledWithStringViewArgThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const std::string_view arg{"x"};
		EXPECT_THROW(static_cast<void>(Logger::info("{} {}", arg)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenInfoCalledWithTwoStringViewArgsThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const std::string_view arg1{"a"};
		const std::string_view arg2{"b"};
		EXPECT_THROW(static_cast<void>(Logger::info("{} {} {}", arg1, arg2)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenInfoCalledWithUnsignedCharArgThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const unsigned char arg{1};
		EXPECT_THROW(static_cast<void>(Logger::info("{} {}", arg)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenWarnCalledWithStringViewArgThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const std::string_view arg{"x"};
		EXPECT_THROW(static_cast<void>(Logger::warn("{} {}", arg)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenWarnCalledWithTwoStringViewArgsThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const std::string_view arg1{"a"};
		const std::string_view arg2{"b"};
		EXPECT_THROW(static_cast<void>(Logger::warn("{} {} {}", arg1, arg2)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenWarnCalledWithUnsignedCharAndStringViewArgsThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const unsigned char userID{1};
		const std::string_view name{"x"};
		EXPECT_THROW(static_cast<void>(Logger::warn("{} {} {}", userID, name)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenWarnCalledWithTwoUnsignedCharArgsThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const unsigned char id1{1};
		const unsigned char id2{2};
		EXPECT_THROW(static_cast<void>(Logger::warn("{} {} {}", id1, id2)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenWarnCalledWithUnsignedLongAndUnsignedCharArgsThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });
		const unsigned char userID{1};
		EXPECT_THROW(static_cast<void>(Logger::warn("{} {} {}", 20UL, userID)), std::runtime_error);
		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	// ────────────────────────────────────────────────────────────
	// truncateFile parameter
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenExistingContentWhenInitializedWithTruncateThenFileIsCleared)
	{
		// Write some content through the logger so the file is non-empty
		auto preResult{Logger::info("pre-existing content")};
		EXPECT_FALSE(preResult.has_value());
		spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

		std::string contents{readLogFile()};
		ASSERT_FALSE(contents.empty());

		// Re-initialize with truncateFile {tru}e
		spdlog::drop_all();
		bool initResult{Logger::initialize(getLoggerName(), getLogFileName(), true)};
		EXPECT_TRUE(initResult);

		auto postResult{Logger::info("after truncate")};
		EXPECT_FALSE(postResult.has_value());
		contents = readLogFile();

		EXPECT_EQ(contents.find("pre-existing content"), std::string::npos);
		EXPECT_NE(contents.find("after truncate"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenExistingContentWhenInitializedWithoutTruncateThenFileIsPreserved)
	{
		auto keepResult{Logger::info("keep this content")};
		EXPECT_FALSE(keepResult.has_value());
		spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

		// Re-initialize without truncation (default)
		spdlog::drop_all();
		bool initResult{Logger::initialize("preserve_logger", getLogFileName(), false)};
		EXPECT_TRUE(initResult);

		auto appendResult{Logger::info("appended content")};
		EXPECT_FALSE(appendResult.has_value());
		spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &logger) { logger->flush(); });

		std::string contents{readLogFile()};
		EXPECT_NE(contents.find("keep this content"), std::string::npos);
		EXPECT_NE(contents.find("appended content"), std::string::npos);
	}

	TEST_F(LoggerTest, GivenInvalidPathWhenInitializedWithTruncateThenReturnsFalse)
	{
		spdlog::drop_all();
		// A path under a non-existent directory should fail to open for truncation
		bool initResult{Logger::initialize("trunc_fail_logger", "/no_such_dir/no_such_subdir/fail.log", true)};
		EXPECT_FALSE(initResult);
	}

	// ────────────────────────────────────────────────────────────
	// spdlog initialization failure (duplicate name without drop)
	// ────────────────────────────────────────────────────────────

	TEST_F(LoggerTest, GivenDuplicateRegistryNameWhenInitializedThenReturnsFalse)
	{
		// Manually register a logger with the same name so spdlog::basic_logger_mt throws
		std::string duplicateName{"duplicate_logger"};
		std::string tempFile{"logger_test_dup.log"};
		const std::shared_ptr<spdlog::logger> manualLogger{spdlog::basic_logger_mt(duplicateName, tempFile)};

		// initialize will reset its own internal state but cannot drop the manually registered logger,
		// causing spdlog to throw spdlog_ex which is caught internally and returns false.
		bool initResult{Logger::initialize(duplicateName, getLogFileName())};
		EXPECT_FALSE(initResult);

		spdlog::drop(duplicateName);
		static_cast<void>(std::filesystem::remove(tempFile));
	}

	// ────────────────────────────────────────────────────────────
	// Logging failure — returns error string_view on spdlog_ex
	// ────────────────────────────────────────────────────────────
	// spdlog internally catches all std::exception in its log methods
	// and calls its error handler. To make spdlog_ex propagate out
	// to Logger's catch blocks, we set a custom error handler that
	// re-throws as spdlog_ex, then trigger a format error with a
	// mismatched format string (placeholder with no arguments).

	TEST_F(LoggerTest, GivenSpdlogExWhenLogCalledThenReturnsLogLogFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::log(spdlog::level::info, "{} {}", 77)};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::LOG_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenTraceCalledThenReturnsTraceLogFailure)
	{
		Logger::setLevel(spdlog::level::trace);
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::trace("{} {}", 42)};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::TRACE_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenDebugCalledThenReturnsDebugLogFailure)
	{
		Logger::setLevel(spdlog::level::debug);
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::debug("{} {}", "hello")};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::DEBUG_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenInfoCalledThenReturnsInfoLogFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::info("{} {}", 100)};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::INFO_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenWarnCalledThenReturnsWarnLogFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::warn("{} {}", 3.14)};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::WARN_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenErrorCalledThenReturnsErrorLogFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::error("{} {}", "failure")};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::ERROR_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenSpdlogExWhenCriticalCalledThenReturnsCriticalLogFailure)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw spdlog::spdlog_ex(msg); });

		std::optional<std::string_view> result{Logger::critical("{} {}", 999)};

		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(result.value(), Project::Utility::Debug::Logging::CRITICAL_LOG_FAILURE);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	// ────────────────────────────────────────────────────────────
	// Logging failure — non-spdlog_ex propagates past the catch
	// ────────────────────────────────────────────────────────────
	// GCC generates a branch on each catch clause for "exception
	// does not match this handler". Throwing std::runtime_error
	// exercises that path.

	TEST_F(LoggerTest, GivenNonSpdlogExWhenLogCalledThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::log(spdlog::level::info, "{} {}", 77)), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenTraceCalledThenExceptionPropagates)
	{
		Logger::setLevel(spdlog::level::trace);
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::trace("{} {}", 42)), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenDebugCalledThenExceptionPropagates)
	{
		Logger::setLevel(spdlog::level::debug);
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::debug("{} {}", "hello")), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenInfoCalledThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::info("{} {}", 100)), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenWarnCalledThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::warn("{} {}", 3.14)), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenErrorCalledThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::error("{} {}", "failure")), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}

	TEST_F(LoggerTest, GivenNonSpdlogExWhenCriticalCalledThenExceptionPropagates)
	{
		spdlog::set_error_handler([] ATTR_NORETURN(const std::string &msg) { throw std::runtime_error(msg); });

		EXPECT_THROW(static_cast<void>(Logger::critical("{} {}", 999)), std::runtime_error);

		spdlog::set_error_handler([](const std::string & /*msg*/) {});
	}
} // namespace

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
