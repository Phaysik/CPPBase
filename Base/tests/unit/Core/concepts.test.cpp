/*! @file concepts.test.cpp
	@brief C++ file for creating tests for validating custom concepts.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#include "Core/concepts.h"

#include <cstddef>
#include <functional>
#include <ratio>
#include <string>
#include <string_view>

#include "Core/typedefs.h"

#include <catch2/catch_test_macros.hpp>

using Project::Core::FloatingPoint;
using Project::Core::Integral;
using Project::Core::InvocableNoArgs;
using Project::Core::InvocableWithArgs;
using Project::Core::IsEnum;
using Project::Core::Ratio;
using Project::Core::RationalNumber;
using Project::Core::sb;
using Project::Core::si;
using Project::Core::SignedIntegral;
using Project::Core::sl;
using Project::Core::String;
using Project::Core::ub;
using Project::Core::ui;
using Project::Core::ul;
using Project::Core::UnsignedIntegral;

// Compile-time sanity checks (will fail to compile if concepts change unexpectedly)
static_assert(Integral<si>);
static_assert(Integral<bool>);
static_assert(FloatingPoint<double>);
static_assert(!FloatingPoint<si>);
static_assert(RationalNumber<si>);
static_assert(RationalNumber<double>);
static_assert(String<std::string>);
static_assert(String<const std::string &>);
static_assert(!String<const char *>);

// NOLINTBEGIN(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)

SCENARIO("Concepts")
{
	GIVEN("Integral")
	{
		GIVEN("Basic integral types")
		{
			THEN("They should satisfy the Integral concept")
			{
				REQUIRE(Integral<si>);
				REQUIRE(Integral<ul>);
				REQUIRE(Integral<bool>);
			}
		}

		GIVEN("Non-integral types")
		{
			THEN("They should not satisfy the Integral concept")
			{
				REQUIRE_FALSE(Integral<double>);
				REQUIRE_FALSE(Integral<std::string>);
			}
		}
	}

	GIVEN("Signed Integrals")
	{
		GIVEN("Signed integral types")
		{
			THEN("They should satisfy the appropriate concepts")
			{
				REQUIRE(SignedIntegral<sl>);
				REQUIRE(SignedIntegral<si>);
				REQUIRE(SignedIntegral<sb>);
			}
		}

		GIVEN("Unsigned integral types")
		{
			THEN("They should not satisfy the SignedIntegral concept")
			{
				REQUIRE_FALSE(SignedIntegral<ul>);
				REQUIRE_FALSE(SignedIntegral<ui>);
				REQUIRE_FALSE(SignedIntegral<ub>);
			}
		}

		GIVEN("Non-Integral types")
		{
			THEN("They should not satisfy the UnsignedIntegral concept")
			{
				REQUIRE_FALSE(UnsignedIntegral<double>);
				REQUIRE_FALSE(UnsignedIntegral<std::string>);
			}
		}
	}

	GIVEN("Unsigned Integrals")
	{
		GIVEN("Unsigned integral types")
		{
			THEN("They should satisfy the appropriate concepts")
			{
				REQUIRE(UnsignedIntegral<ul>);
				REQUIRE(UnsignedIntegral<ui>);
				REQUIRE(UnsignedIntegral<ub>);
			}
		}

		GIVEN("Signed integral types")
		{
			THEN("They should not satisfy the UnsignedIntegral concept")
			{
				REQUIRE_FALSE(UnsignedIntegral<sl>);
				REQUIRE_FALSE(UnsignedIntegral<si>);
				REQUIRE_FALSE(UnsignedIntegral<sb>);
			}
		}

		GIVEN("Non-Integral types")
		{
			THEN("They should not satisfy the UnsignedIntegral concept")
			{
				REQUIRE_FALSE(UnsignedIntegral<double>);
				REQUIRE_FALSE(UnsignedIntegral<std::string>);
			}
		}
	}

	GIVEN("Floating Point")
	{
		GIVEN("Floating-point types")
		{
			THEN("They should satisfy the FloatingPoint concept")
			{
				REQUIRE(FloatingPoint<float>);
				REQUIRE(FloatingPoint<double>);
			}
		}

		GIVEN("Non-floating-point types")
		{
			THEN("They should not satisfy the FloatingPoint concept")
			{
				REQUIRE_FALSE(FloatingPoint<si>);
				REQUIRE_FALSE(FloatingPoint<std::string>);
			}
		}
	}

	GIVEN("Rational Number")
	{
		GIVEN("Integral and floating-point types")
		{
			THEN("They should satisfy the RationalNumber concept")
			{
				REQUIRE(RationalNumber<si>);
				REQUIRE(RationalNumber<double>);
				REQUIRE(RationalNumber<bool>);
			}
		}

		GIVEN("Non-integral and non-floating-point types")
		{
			THEN("They should not satisfy the RationalNumber concept")
			{
				REQUIRE_FALSE(RationalNumber<std::string>);
				REQUIRE_FALSE(RationalNumber<std::string_view>);
			}
		}
	}

	// NOLINTBEGIN(hicpp-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)

	GIVEN("String")
	{
		GIVEN("std::string and its cv-ref variants")
		{
			THEN("They should satisfy the String concept")
			{
				REQUIRE(String<std::string>);
				REQUIRE(String<const std::string &>);
				REQUIRE(String<std::string &&>);
			}
		}

		GIVEN("Non-std::string types")
		{
			THEN("They should not satisfy the String concept")
			{
				REQUIRE_FALSE(String<std::string_view>);
				REQUIRE_FALSE(String<const char *>);
				REQUIRE_FALSE(String<char[6]>);
			}
		}
	}

	// NOLINTEND(hicpp-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)

	GIVEN("InvocableNoArgs")
	{
		GIVEN("std::function<void()> and a lambda with no arguments")
		{
			THEN("They should satisfy the InvocableNoArgs concept")
			{
				REQUIRE(InvocableNoArgs<std::function<void()>>);
				REQUIRE(InvocableNoArgs<decltype([] {})>);
			}
		}

		GIVEN("std::function<void(param)> and a lambda with arguments")
		{
			THEN("They should not satisfy the InvocableNoArgs concept")
			{
				REQUIRE_FALSE(InvocableNoArgs<std::function<void(std::size_t)>>);
				REQUIRE_FALSE(InvocableNoArgs<decltype([](std::size_t) {})>);
			}
		}

		GIVEN("Non-callable types")
		{
			THEN("Should not satisfy the InvocableNoArgs concept")
			{
				REQUIRE_FALSE(InvocableNoArgs<std::string>);
				REQUIRE_FALSE(InvocableNoArgs<std::string_view>);
			}
		}
	}

	GIVEN("InvocableWithArgs")
	{
		GIVEN("std::function<void(param?)> and a lambda with arguments")
		{
			THEN("They should satisfy the InvocableWithArgs concept")
			{
				REQUIRE(InvocableWithArgs<std::function<void(std::size_t)>, std::size_t>);
				REQUIRE(InvocableWithArgs<decltype([](std::size_t) {}), std::size_t>);
			}
		}

		GIVEN("std::function<void(param?)> and a lambda with arguments with the wrong argument type")
		{
			THEN("Should not satisfy the InvocableWithArgs concept")
			{
				REQUIRE_FALSE(InvocableWithArgs<std::function<void(std::size_t)>, std::string>);
				REQUIRE_FALSE(InvocableWithArgs<decltype([](std::size_t) {}), std::string>);
			}
		}

		GIVEN("Non-callable types")
		{
			THEN("Should not satisfy the InvocableWithArgs concept")
			{
				REQUIRE_FALSE(InvocableWithArgs<std::string>);
				REQUIRE_FALSE(InvocableWithArgs<std::string_view>);
			}
		}
	}

	GIVEN("Enum")
	{
		GIVEN("Enum and enum class types")
		{
			// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
			enum test
			{
			};

			enum class testClass
			{
			};

			THEN("They should satisfy the Enum concept")
			{
				REQUIRE(IsEnum<test>);
				REQUIRE(IsEnum<testClass>);
			}
		}

		GIVEN("Non-enum and non-enum class types")
		{
			THEN("They should not satisfy the Enum concept")
			{
				REQUIRE_FALSE(IsEnum<std::string>);
				REQUIRE_FALSE(IsEnum<std::string_view>);
			}
		}
	}

	GIVEN("Ratio")
	{
		GIVEN("normalized std::ratio specializations")
		{
			THEN("they satisfy the Ratio concept")
			{
				REQUIRE((Ratio<std::ratio<1, 2>>) );
				REQUIRE((Ratio<std::ratio<-1, 2>>) );
				REQUIRE(Ratio<std::ratio<0>>);
				REQUIRE(Ratio<std::ratio<1>>);
				REQUIRE(Ratio<std::milli>);
			}
		}

		GIVEN("unreduced ratios, cv-ref variants, and types without ratio members")
		{
			THEN("they do not satisfy the Ratio concept")
			{
				REQUIRE_FALSE((Ratio<std::ratio<2, 4>>) );
				REQUIRE_FALSE((Ratio<std::ratio<0, 5>>) );
				REQUIRE_FALSE((Ratio<std::ratio<1, -2>>) );
				REQUIRE_FALSE((Ratio<const std::ratio<1, 2>>) );
				REQUIRE_FALSE((Ratio<volatile std::ratio<1, 2>>) );
				REQUIRE_FALSE((Ratio<std::ratio<1, 2> &>) );
				REQUIRE_FALSE((Ratio<std::ratio<1, 2> &&>) );
				REQUIRE_FALSE(Ratio<si>);
				REQUIRE_FALSE(Ratio<void>);
				REQUIRE_FALSE(Ratio<std::string>);
				REQUIRE_FALSE(Ratio<std::string_view>);
			}
		}
	}

	GIVEN("RandomFn")
	{
		GIVEN("std::function and a lambda that accept and return the specified type")
		{
			THEN("they should satisfy the RandomFn concept")
			{
				REQUIRE(RandomFn<std::function<double(double, double)>, double>);
				REQUIRE(RandomFn<decltype([](double, double) { return 1.0; }), double>);
			}
		}

		GIVEN("callables with an incompatible return type or argument list")
		{
			THEN("they should not satisfy the RandomFn concept")
			{
				REQUIRE_FALSE(RandomFn<std::function<float(double, double)>, double>);
				REQUIRE_FALSE(RandomFn<std::function<void(double, double)>, double>);
				REQUIRE_FALSE(RandomFn<std::function<double(std::string, std::string)>, double>);
			}
		}

		GIVEN("a non-callable type")
		{
			THEN("it should not satisfy the RandomFn concept")
			{
				REQUIRE_FALSE(RandomFn<std::string, double>);
			}
		}
	}
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)
