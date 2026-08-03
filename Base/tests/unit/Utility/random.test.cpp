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

using Project::Core::us;
using Project::Utility::Random;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,bugprone-random-generator-seed,cert-msc51-cpp,cert-msc32-c,readability-function-cognitive-complexity)

SCENARIO("Random")
{
	// Reseed with a fixed value before each scenario for determinism
	Random::getTwister().seed(12'345U);

	GIVEN("a signed integer range [1, 10]")
	{
		WHEN("get is called")
		{
			int result{Random::get<int>(1, 10)};

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
			int result{Random::get<int>(7, 7)};

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

			for (int idx{0}; idx < 100; ++idx)
			{
				int val{Random::get<int>(-50, 50)};

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

	GIVEN("the global twister accessed via getTwister")
	{
		WHEN("the twister is reseeded to a known state and get is called twice")
		{
			std::mt19937 &twister{Random::getTwister()};

			twister.seed(99U); // Fixed seed — first draw
			int firstResult{Random::get<int>(0, 10'000)};

			twister.seed(99U); // Same seed — second draw must match
			int secondResult{Random::get<int>(0, 10'000)};

			THEN("both draws produce the same value")
			{
				CHECK((firstResult == secondResult));
			}
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,bugprone-random-generator-seed,cert-msc51-cpp,cert-msc32-c,readability-function-cognitive-complexity)
