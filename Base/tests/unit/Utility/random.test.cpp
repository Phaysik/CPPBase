/*! @file random.test.cpp
	@brief Catch2 unit tests for `Utility::Random` utilities.
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#include "Utility/random.h"

#include "Core/typedefs.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using PocketCore::Core::sb;
using PocketCore::Core::ub;
using PocketCore::Core::us;
using PocketCore::Utility::Random;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,bugprone-random-generator-seed,cert-msc51-cpp,cert-msc32-c,readability-function-cognitive-complexity)

SCENARIO("Random")
{
	// Reseed with a fixed value before each scenario for determinism
	Random::getTwister().seed(12'345U);

	GIVEN("a signed integer range [1, 10]")
	{
		WHEN("get is called")
		{
			ub result{Random::get<ub>(1, 10)};

			THEN("the result is within [1, 10]")
			{
				CHECK((result >= 1));
				CHECK((result <= 10));
			}
		}
	}

	GIVEN("a degenerate range where min equals max")
	{
		WHEN("get is called with min == max")
		{
			ub result{Random::get<ub>(7, 7)};

			THEN("the only possible value is returned")
			{
				CHECK((result == 7));
			}
		}
	}

	GIVEN("an unsigned short range [0, 100]")
	{
		WHEN("get is called with an unsigned short type")
		{
			us result{Random::get<us>(0U, 100U)};

			THEN("the result is within [0, 100]")
			{
				CHECK((result <= 100U));
			}
		}
	}

	GIVEN("multiple draws over a wide signed range [-50, 50]")
	{
		WHEN("one hundred values are drawn")
		{
			bool allInRange{true};

			for (ub idx{0}; idx < 100; ++idx)
			{
				sb val{Random::get<sb>(-50, 50)};

				if (val < -50 || val > 50)
				{
					allInRange = false;
				}
			}

			THEN("every value is within the bounds")
			{
				CHECK(allInRange);
			}
		}
	}

	GIVEN("a floating point range [1.0, 10.0)")
	{
		WHEN("get is called with a double type")
		{
			double result{Random::get<double>(0.45, 0.63)};

			THEN("the result is within [0.45, 0.63)")
			{
				CHECK((result >= 0.45));
				CHECK((result < 0.63));
			}
		}
	}

	GIVEN("the global twister accessed via getTwister")
	{
		WHEN("the twister is reseeded to a known state and get is called twice for integers")
		{
			std::mt19937 &twister{Random::getTwister()};

			twister.seed(99U); // Fixed seed — first draw
			us firstResult{Random::get<us>(0, 10'000)};

			twister.seed(99U); // Same seed — second draw must match
			us secondResult{Random::get<us>(0, 10'000)};

			THEN("both draws produce the same value")
			{
				CHECK((firstResult == secondResult));
			}
		}

		WHEN("the twister is reseeded to a known state and get is called twice for doubles")
		{
			std::mt19937 &twister{Random::getTwister()};

			twister.seed(99U); // Fixed seed — first draw
			double firstResult{Random::get<double>(0.45, 0.63)};

			twister.seed(99U); // Same seed — second draw must match
			double secondResult{Random::get<double>(0.45, 0.63)};

			THEN("both draws produce the same value")
			{
				CHECK_THAT(firstResult, Catch::Matchers::WithinAbs(secondResult, 1e-9));
			}
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,bugprone-random-generator-seed,cert-msc51-cpp,cert-msc32-c,readability-function-cognitive-complexity)
