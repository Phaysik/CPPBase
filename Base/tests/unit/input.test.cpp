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

SCENARIO("Input getInput direct")
{
	GIVEN("a valid integer on the stream")
	{
		std::istringstream input{"42\n"};

		WHEN("getInput is called directly with a non-empty prompt")
		{
			int result{Input::getInput<int>("Enter: ", {}, true, input)};

			THEN("the value is returned")
			{
				CHECK((result == 42));
			}
		}
	}

	GIVEN("invalid input followed by valid input")
	{
		std::istringstream input{"abc\n42\n"};

		WHEN("getInput is called with a non-empty error message")
		{
			int result{Input::getInput<int>({}, "Invalid!", true, input)};

			THEN("the valid value is returned after retry")
			{
				CHECK((result == 42));
			}
		}
	}

	GIVEN("input with extraneous characters after the number")
	{
		std::istringstream input{"42abc\n99\n"};

		WHEN("getInput is called with ignoreExtraneous set")
		{
			int result{Input::getInput<int>({}, "Invalid!", true, input)};

			THEN("extraneous input is rejected and the next clean value is returned")
			{
				CHECK((result == 99));
			}
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)