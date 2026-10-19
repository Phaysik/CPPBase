/*! @file callableRandomSource.test.cpp
	@brief C++ file for running tests for the CallableRandomSource.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#include "Utility/Random/callableRandomSource.h"

#include <functional>
#include <memory>
#include <utility>

#include "Core/typedefs.h"
#include "Utility/Random/randomSourceInterface.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Project::Core::si;
using Project::Utility::Random::CallableRandomSource;
using Project::Utility::Random::IRandomSource;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)

SCENARIO("CallableRandomSource")
{
	GIVEN("an integral callable that records its arguments and invocation count")
	{
		si receivedMin{0};
		si receivedMax{0};
		si callCount{0};
		auto callable = [&](si min, si max) -> si {
			receivedMin = min;
			receivedMax = max;
			++callCount;
			return 42;
		};
		CallableRandomSource<si, decltype(callable)> source{callable};

		THEN("construction does not invoke the callable")
		{
			CHECK((callCount == 0));
		}

		WHEN("generation is requested through the random source interface")
		{
			IRandomSource<si> &interface{source};
			si result{interface(-10, 10)};

			THEN("the callable is invoked once with unchanged bounds and its result is returned unchanged")
			{
				CHECK((receivedMin == -10));
				CHECK((receivedMax == 10));
				CHECK((callCount == 1));
				CHECK((result == 42));
			}
		}

		WHEN("the bounds are equal")
		{
			si result{source(7, 7)};

			THEN("the callable still determines the result")
			{
				CHECK((receivedMin == 7));
				CHECK((receivedMax == 7));
				CHECK((callCount == 1));
				CHECK((result == 42));
			}
		}

		WHEN("the callable accepts reversed bounds")
		{
			si result{source(10, -10)};

			THEN("the adapter forwards the bounds without reordering or validation")
			{
				CHECK((receivedMin == 10));
				CHECK((receivedMax == -10));
				CHECK((callCount == 1));
				CHECK((result == 42));
			}
		}
	}

	GIVEN("a floating-point callable")
	{
		double receivedMin{0.0};
		double receivedMax{0.0};
		auto callable = [&](double min, double max) -> double {
			receivedMin = min;
			receivedMax = max;
			return 12.5;
		};
		CallableRandomSource<double, decltype(callable)> source{callable};

		THEN("bounds and the result are forwarded without conversion or clamping")
		{
			IRandomSource<double> &interface{source};
			double result{interface(-2.5, 1.25)};

			CHECK_THAT(receivedMin, Catch::Matchers::WithinAbs(-2.5, 0.0));
			CHECK_THAT(receivedMax, Catch::Matchers::WithinAbs(1.25, 0.0));
			CHECK_THAT(result, Catch::Matchers::WithinAbs(12.5, 0.0));
		}
	}

	GIVEN("a mutable callable with value-owned state")
	{
		auto callable = [count = si{0}](si min, si /* max */) mutable -> si { return min + ++count; };
		CallableRandomSource<si, decltype(callable)> source{callable};

		THEN("state persists between calls without modifying the original callable")
		{
			CHECK((source(10, 20) == 11));
			CHECK((source(10, 20) == 12));
			CHECK((callable(10, 20) == 11));
		}
	}

	GIVEN("a move-only callable")
	{
		auto callable = [value = std::make_unique<si>(7)](si min, si /* max */) -> si { return min + *value; };
		CallableRandomSource<si, decltype(callable)> source{std::move(callable)};

		THEN("the callable is moved into storage and remains usable")
		{
			CHECK((source(10, 20) == 17));
			CHECK((source(20, 30) == 27));
		}
	}

	GIVEN("a reference wrapper around a stateful callable")
	{
		auto callable = [count = si{0}](si /* min */, si /* max */) mutable -> si { return ++count; };
		CallableRandomSource<si, std::reference_wrapper<decltype(callable)>> source{std::ref(callable)};

		THEN("invocation updates the referenced callable rather than a copy")
		{
			CHECK((source(0, 10) == 1));
			CHECK((callable(0, 10) == 2));
			CHECK((source(0, 10) == 3));
		}
	}

	GIVEN("a function pointer")
	{
		auto callable = +[](si /* min */, si max) -> si { return max; };
		CallableRandomSource<si, decltype(callable)> source{callable};

		THEN("the function is invoked with the supplied bounds")
		{
			CHECK((source(-10, 10) == 10));
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)
