/*! @file defaultRandomSource.test.cpp
	@brief C++ file for running tests for the DefaultRandomSource.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#include "Utility/Random/defaultRandomSource.h"

#include <random>
#include <cstddef>
#include <limits>

#include "Core/typedefs.h"
#include "Utility/Random/random.h"
#include "Utility/Random/randomSourceInterface.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Project::Core::si;
using Project::Utility::Random::DefaultRandomSource;
using Project::Utility::Random::getDefaultRandomSource;
using Project::Utility::Random::IRandomSource;
using Project::Utility::Random::Random;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)

namespace
{
	struct RandomStateGuard
	{
		public:
			RandomStateGuard() = default;

			RandomStateGuard(const RandomStateGuard &) = delete;

			RandomStateGuard(RandomStateGuard &&) = delete;

			RandomStateGuard &operator=(const RandomStateGuard &) = delete;

			RandomStateGuard &operator=(RandomStateGuard &&) = delete;

			~RandomStateGuard()
			{
				Random::getTwister() = savedState;
			}

		public:
			// NOLINTBEGIN(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
			std::mt19937 savedState{Random::getTwister()};
			// NOLINTEND(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
	};
} // namespace

SCENARIO("DefaultRandomSource")
{
	RandomStateGuard stateGuard{};

	GIVEN("the default index source")
	{
		IRandomSource<std::size_t> &source{getDefaultRandomSource<std::size_t>()};

		THEN("generation delegates to the shared utility without narrowing indices")
		{
			constexpr std::size_t maximum{std::numeric_limits<std::size_t>::max()};
			constexpr std::size_t minimum{maximum - 100};
			Random::setSeed(42UL);
			const std::size_t expected{Random::get<std::size_t>(minimum, maximum)};
			Random::setSeed(42UL);

			const std::size_t result{source(minimum, maximum)};
			CHECK((result == expected));
			CHECK((result >= minimum));
			CHECK((result <= maximum));
			CHECK((source(maximum, maximum) == maximum));
		}
	}

	GIVEN("an integral default random source")
	{
		DefaultRandomSource<si> source{};

		THEN("generated values stay within the inclusive bounds")
		{
			for (si draw{0}; draw < 100; ++draw)
			{
				si result{source(-10, 10)};
				CHECK((result >= -10));
				CHECK((result <= 10));
			}
		}

		THEN("equal bounds return the only possible value")
		{
			CHECK((source(7, 7) == 7));
		}

		THEN("generation delegates to the shared random utility")
		{
			Random::setSeed(42UL);
			si expectedFirst{Random::get<si>(-10, 10)};
			si expectedSecond{Random::get<si>(-10, 10)};
			Random::setSeed(42UL);

			CHECK((source(-10, 10) == expectedFirst));
			CHECK((source(-10, 10) == expectedSecond));
		}
	}

	GIVEN("a floating-point default random source")
	{
		DefaultRandomSource<double> source{};

		THEN("generated values stay within the half-open bounds")
		{
			for (int draw{0}; draw < 100; ++draw)
			{
				double result{source(-1.0, 1.0)};
				CHECK((result >= -1.0));
				CHECK((result < 1.0));
			}
		}

		THEN("equal bounds return the only possible value")
		{
			CHECK_THAT(source(0.5, 0.5), Catch::Matchers::WithinAbs(0.5, 0.0));
		}

		THEN("generation delegates to the shared random utility")
		{
			Random::setSeed(42UL);
			double expectedFirst{Random::get<double>(-1.0, 1.0)};
			double expectedSecond{Random::get<double>(-1.0, 1.0)};
			Random::setSeed(42UL);

			CHECK_THAT(source(-1.0, 1.0), Catch::Matchers::WithinAbs(expectedFirst, 0.0));
			CHECK_THAT(source(-1.0, 1.0), Catch::Matchers::WithinAbs(expectedSecond, 0.0));
		}
	}
}

SCENARIO("getDefaultRandomSource")
{
	RandomStateGuard stateGuard{};

	GIVEN("the default sources for integral and floating-point types")
	{
		IRandomSource<si> &integralSource{getDefaultRandomSource<si>()};
		IRandomSource<double> &floatingSource{getDefaultRandomSource<double>()};

		THEN("repeated calls return the same instance for each type")
		{
			CHECK((&integralSource == &getDefaultRandomSource<si>()));
			CHECK((&floatingSource == &getDefaultRandomSource<double>()));
		}

		THEN("the returned interfaces generate values for equal bounds")
		{
			CHECK((integralSource(7, 7) == 7));
			CHECK_THAT(floatingSource(0.5, 0.5), Catch::Matchers::WithinAbs(0.5, 0.0));
		}

		THEN("sources of different types advance the same shared generator")
		{
			Random::setSeed(42UL);
			si expectedIntegral{Random::get<si>(-10, 10)};
			double expectedFloating{Random::get<double>(-1.0, 1.0)};
			si expectedNextIntegral{Random::get<si>(-10, 10)};
			Random::setSeed(42UL);

			CHECK((integralSource(-10, 10) == expectedIntegral));
			CHECK_THAT(floatingSource(-1.0, 1.0), Catch::Matchers::WithinAbs(expectedFloating, 0.0));
			CHECK((getDefaultRandomSource<si>()(-10, 10) == expectedNextIntegral));
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)
