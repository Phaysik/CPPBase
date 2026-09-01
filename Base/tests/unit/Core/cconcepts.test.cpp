/*! @file cconcepts.test.cpp
	@brief C++ file for creating tests for validating custom concepts.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#include "Core/cconcepts.h"

#include <functional>
#include <string>
#include <string_view>

#include "Core/typedefs.h"

#include <catch2/catch_test_macros.hpp>

using PocketCore::Core::FloatingPoint;
using PocketCore::Core::Integral;
using PocketCore::Core::InvocableNoArgs;
using PocketCore::Core::InvocableWithArgs;
using PocketCore::Core::IsEnum;
using PocketCore::Core::RationalNumber;
using PocketCore::Core::sb;
using PocketCore::Core::si;
using PocketCore::Core::SignedIntegral;
using PocketCore::Core::sl;
using PocketCore::Core::String;
using PocketCore::Core::ub;
using PocketCore::Core::ui;
using PocketCore::Core::ul;
using PocketCore::Core::UnsignedIntegral;

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
}

// NOLINTEND(misc-const-correctness,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-function-cognitive-complexity)
