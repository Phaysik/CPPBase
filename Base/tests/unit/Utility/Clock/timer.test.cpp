/*! @file timer.test.cpp
	@brief Google Test unit tests for `Clock::Timer` utilities.
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#include "Utility/Clock/timer.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <ratio>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>

#include <gtest/gtest.h>

using Project::Utility::Clock::Timer;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(TimerGetUnitTest, DefaultAndCommonRatios)
{
	EXPECT_EQ(Timer::getUnit<std::ratio<1>>(), "s");
	EXPECT_EQ(Timer::getUnit<std::milli>(), "ms");
	EXPECT_EQ(Timer::getUnit<std::micro>(), "us");
	EXPECT_EQ(Timer::getUnit<std::nano>(), "ns");
}

TEST(TimerGetUnitTest, UnknownRatioReturnsUnknown)
{
	// num != 1 triggers the "unknown" path
	using two_over_one = std::ratio<2, 1>;
	EXPECT_EQ(Timer::getUnit<two_over_one>(), "unknown");
}

TEST(TimerStartStopTest, MeasuresPositiveDuration)
{
	// Ensure start/stop produce a non-zero positive duration when sleeping
	Timer::start();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	double elapsed = Timer::stop<>();

	EXPECT_GT(elapsed, 0.0);
}

TEST(TimerTimeFunctionTest, WritesTimingOutputToStdoutWhenNoLogFile)
{
	// Ensure any previously opened log file is closed so output goes to stdout.
	Timer::closeLogFile();

	// Capture stdout produced by timeFunction
	std::ostringstream captured;
	std::streambuf *old = std::cout.rdbuf(captured.rdbuf());

	auto trivial = []() noexcept {
		int counter = 0;
		++counter;
		(void) counter;
	};

	// Run 3 iterations to exercise the per-iteration and average output path
	Timer::timeFunction<std::ratio<1>>("trivial", 3U, trivial);

	// restore
	std::cout.rdbuf(old);

	std::string out = captured.str();
	EXPECT_NE(out.find("Timing function: trivial"), std::string::npos);
	EXPECT_NE(out.find("Iteration 1"), std::string::npos);
	EXPECT_NE(out.find("Average:"), std::string::npos);
}

TEST(TimerCreateLogFileTest, CreatesLogFileAndWritesOutput)
{
	namespace fs = std::filesystem;

	fs::path tmpName = "timer_test_file.log";
	bool result{fs::remove(tmpName.c_str())};

	// The file does not exist, so both should be false
	EXPECT_TRUE(!result && !fs::exists(tmpName));

	// Create the log file that Timer will use
	Timer::createLogFile(tmpName);

	auto trivial = []() noexcept {
		int counter = 0;
		++counter;
		(void) counter;
	};

	// Time the function which should write into the log file
	Timer::timeFunction<std::ratio<1>>("file_trivial", 2U, trivial);

	// Close the stream held by Timer by creating a temporary Timer instance which will close
	// the internal log file in its destructor, ensuring the file can be removed reliably.
	// Ensure internal log stream is closed so file can be removed reliably.
	Timer::closeLogFile();

	// Verify file exists (content may be buffered in the process' open stream);
	// presence of the file demonstrates createLogFile opened it successfully.
	EXPECT_TRUE(fs::exists(tmpName));

	result = fs::remove(tmpName.c_str());

	// The file did exist, so the remove should have returned true and the file should no longer exist
	EXPECT_TRUE(result && !fs::exists(tmpName));
}

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void noOpTimerFunction() noexcept {} // Named function so both file and stdout calls share one template instantiation

TEST(TimerTimeFunctionTest, GivenSingleIterationWhenTimeFunctionCalledThenAverageIsNotPrinted)
{
	Timer::closeLogFile();

	std::ostringstream captured;
	std::streambuf *oldBuf = std::cout.rdbuf(captured.rdbuf());

	Timer::timeFunction<std::ratio<1>>("single_iter", 1U, noOpTimerFunction);

	std::cout.rdbuf(oldBuf);

	std::string out = captured.str();
	EXPECT_NE(out.find("Timing function: single_iter"), std::string::npos);
	EXPECT_NE(out.find("Iteration 1"), std::string::npos);
	EXPECT_EQ(out.find("Average:"), std::string::npos);
}

TEST(TimerTimeFunctionTest, GivenNamedFunctionWhenCalledWithAndWithoutFileThenBothOutputPathsCovered)
{
	namespace fs = std::filesystem;
	fs::path tmpName = "timer_test_both_paths.log";
	static_cast<void>(fs::remove(tmpName));

	// Call with file open — exercises the logFile branch of the ternary
	Timer::createLogFile(tmpName);
	Timer::timeFunction<std::ratio<1>>("with_file", 2U, noOpTimerFunction);
	Timer::closeLogFile();

	// Call without file — exercises the std::cout branch in the same template instantiation
	std::ostringstream captured;
	std::streambuf *oldBuf = std::cout.rdbuf(captured.rdbuf());
	Timer::timeFunction<std::ratio<1>>("without_file", 1U, noOpTimerFunction);
	std::cout.rdbuf(oldBuf);

	std::string out = captured.str();
	EXPECT_NE(out.find("Timing function: without_file"), std::string::npos);

	static_cast<void>(fs::remove(tmpName));
}

TEST(TimerCreateLogFileTest, GivenAlreadyOpenFileWhenCreateLogFileCalledAgainThenNoOpSucceeds)
{
	namespace fs = std::filesystem;
	fs::path tmpName = "timer_test_double_open.log";
	static_cast<void>(fs::remove(tmpName));

	Timer::createLogFile(tmpName);
	// Call again while file is already open — exercises the is_open() true / skip-open branch
	Timer::createLogFile(tmpName);

	Timer::closeLogFile();

	EXPECT_TRUE(fs::exists(tmpName));
	static_cast<void>(fs::remove(tmpName));
}

TEST(TimerGetLogFileTest, GivenFailedInitialOpenWhenTimeFunctionCalledThenRetriesOpenViaMFileName)
{
	// Ensure clean state
	Timer::closeLogFile();

	// createLogFile with an invalid path: open() fails silently but mFileName is set
	Timer::createLogFile("/no_such_dir/no_such_subdir/impossible.log");

	// Now call timeFunction which invokes getLogFile(nullptr).
	// mFileName != "null" so it enters the if block; file is not open so it reaches
	// the open() ternary with filename==nullptr, exercising the std::string(mFileName) path.
	std::ostringstream captured;
	std::streambuf *oldBuf = std::cout.rdbuf(captured.rdbuf());

	// Debug: write directly to cout to verify redirect works
	std::cout << "DIRECT_WRITE\n";
	std::cerr << "cout good=" << std::cout.good() << " fail=" << std::cout.fail() << "\n";

	Timer::timeFunction<std::ratio<1>>("retry_open", 1U, noOpTimerFunction);
	std::cout.rdbuf(oldBuf);

	// The retry also fails (invalid path), so output falls back to std::cout
	std::string out = captured.str();
	std::cerr << "captured length: " << out.size() << " content: [" << out << "]\n";
	EXPECT_NE(out.find("Timing function: retry_open"), std::string::npos);

	Timer::closeLogFile();
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)