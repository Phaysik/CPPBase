/*! @file input.test.cpp
	@brief Catch2 unit tests for `Utility::Input` utilities.
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#include "Utility/input.h"

#include <array>
#include <span>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

using Project::Utility::Input;

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

SCENARIO("Input")
{
	GIVEN("an allowed set of ordinary integral values")
	{
		std::array<int, 3> allowedValues{1, 2, 3};
		std::span<const int> allowedSpan{allowedValues};

		WHEN("the first input is allowed")
		{
			std::istringstream input{"2\n"};
			int result{Input::getInput<int>(allowedSpan, {}, {}, true, input)};

			THEN("the selected value is returned")
			{
				CHECK((result == 2));
			}
		}

		WHEN("an input is rejected before an allowed value")
		{
			std::istringstream input{"4\n3\n"};
			int result{Input::getInput<int>(allowedSpan, {}, {}, true, input)};

			THEN("validation retries and returns the allowed value")
			{
				CHECK((result == 3));
			}
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)