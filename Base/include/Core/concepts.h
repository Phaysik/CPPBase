/*! @file concepts.h
	@brief Contains the declarations of common concepts that might be used in multiple files.
	@date --/--/----
	@version x.x.x
	@since x.x.x
	@author Matthew Moore
*/

#ifndef BASE_INCLUDE_CCONCEPTS_H
#define BASE_INCLUDE_CCONCEPTS_H

#include <ratio>
#include <string>
#include <type_traits>

/*! @namespace Project::Core
	@brief Collection of common compile-time type concepts used across the codebase.
	@details This namespace provides small, expressive concepts built on top of the standard
	type-traits library. Use these concepts to constrain template parameters for integral,
	unsigned, signed, floating-point, rational (integral or floating), and std::string-like types.
	@note All concepts are compile-time predicates with no runtime cost.
*/
namespace Project::Core
{
	/*! @concept Integral
		@brief Tests whether a type is an integral type.
		@tparam T The type to test. Typical examples: `int`, `long`, `char`.
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept Integral = std::is_integral_v<T>;

	/*! @concept UnsignedIntegral
		@brief Tests whether a type is an unsigned integral type.
		@tparam T The type to test. Typical examples: `unsigned int`, `std::uint32_t`.
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept UnsignedIntegral = std::is_unsigned_v<T>;

	/*! @concept SignedIntegral
		@brief Tests whether a type is a signed integral type.
		@tparam T The type to test. Typical examples: `int`, `long`.
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept SignedIntegral = std::is_signed_v<T>;

	/*! @concept FloatingPoint
		@brief Tests whether a type is a floating-point type.
		@tparam T The type to test. Typical examples: `float`, `double`, `long double`.
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept FloatingPoint = std::is_floating_point_v<T>;

	/*! @concept RationalNumber
		@brief Tests whether a type represents a rational number (integral or floating-point).
		@details This concept is satisfied if `T` is either an `Integral` or a `FloatingPoint`.
		Use this to constrain templates that accept any numeric type that models rational numbers.
		@tparam T The type to test. Accepts both integral and floating-point types.
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept RationalNumber = Integral<T> || FloatingPoint<T>;

	/*! @concept String
		@brief Tests whether a type is `std::string` (ignores cv/ref qualifiers).
		@details This concept compares the decayed, cv-ref-removed form of `T` with `std::string`.
		It is satisfied for `std::string`, `const std::string&`, `std::string&&`, etc.
		@tparam T The type to test. Use this to constrain templates that require `std::string` specifically.
		@note This concept does not accept C-style strings (e.g., `const char*`).
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept String = std::is_same_v<std::string, std::remove_cvref_t<T>>;

	/*! @concept InvocableNoArgs
		@brief Tests whether a callable type is invocable with no arguments.
		@tparam Func The callable type to test (function, functor, lambda, function pointer, etc.).
		@details This concept is satisfied when `std::invocable<Func>` is true, i.e. the callable
		can be invoked with an empty argument list. Use this to constrain templates that accept
		parameterless callables submitted to thread pools or deferred tasks.
		@since x.x.x
		@version x.x.x
	*/
	template <class Func>
	concept InvocableNoArgs = std::invocable<Func>;

	/*! @concept InvocableWithArgs
		@brief Tests whether a callable type is invocable with the specified argument types.
		@tparam Func The callable type to test (function, functor, lambda, function pointer, etc.).
		@tparam Args The argument types to test invocation with.
		@details This concept is satisfied when `std::invocable<Func, Args...>` is true, i.e. the
		callable can be invoked with the given `Args...`. Use this to constrain templates that accept
		callables and their arguments for asynchronous invocation or task scheduling.
		@since x.x.x
		@version x.x.x
	*/
	template <class Func, class... Args>
	concept InvocableWithArgs = std::invocable<Func, Args...>;

	/*! @concept IsEnum
		@brief Tests whether a type is an enumeration type.
		@tparam T The type to test. Typical examples: scoped enums (`enum class`) and unscoped enums (`enum`).
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
	concept IsEnum = std::is_enum_v<T>;

	/*! @concept Ratio
		@brief Tests whether a type is a normalized std::ratio specialization.
		@details Requires T to be identical to std::ratio<T::num, T::den>, reconstructed from its normalized numerator and denominator.
		@tparam T The type to test; its num and den constants, when present, must form a valid std::ratio specialization.
		@note Does not remove cv/ref qualifiers. std::ratio<1, 2> satisfies the concept, but std::ratio<2, 4> and const std::ratio<1, 2> do
	   not.
		@since x.x.x
		@version x.x.x
		@author Matthew Moore
	*/
	template <typename T>
	concept Ratio = requires {
		T::num;
		T::den;
	} && std::is_same_v<T, std::ratio<T::num, T::den>>;

	/*! @concept RandomFn
		@brief Tests whether a callable accepts two integral or floating-point values and returns that exact type.
		@details Requires @p ValueType to satisfy @ref Integral or @ref FloatingPoint and an lvalue of the callable type to be invocable
	   with two arguments of @p ValueType, with an invocation result exactly matching @p ValueType.
		@tparam Func The callable type to test.
		@tparam ValueType The argument and return type, which must satisfy @ref Integral or @ref FloatingPoint.
		@since x.x.x
		@version x.x.x
	*/
	template <typename Func, typename ValueType>
	concept RandomFn
		= InvocableWithArgs<Func &, ValueType, ValueType> && std::same_as<std::invoke_result_t<Func &, ValueType, ValueType>, ValueType>
	   && (Integral<ValueType> || FloatingPoint<ValueType>);
} // namespace Project::Core

#endif