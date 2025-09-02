/*! \file cconcepts.h
	\brief Contains the declarations of common concepts that might be used in multiple files.
	\date 07/24/2025
	\version x.x.x
	\since x.x.x
	\author Matthew Moore
*/

#ifndef BASE_INCLUDE_CCONCEPTS_H
#define BASE_INCLUDE_CCONCEPTS_H

#include <concepts>
#include <string>
#include <type_traits>

namespace Concepts
{
	template <typename T>
	concept Integral = std::is_integral_v<T>; /*!< Type trait to check if T is an integral type */

	template <typename T>
	concept UnsignedIntegral = std::is_unsigned_v<T>; /*!< Type trait to check if T is an unsigned integral type */

	template <typename T>
	concept SignedIntegral = std::is_signed_v<T>; /*!< Type trait to check if T is a signed integral type */

	template <typename T>
	concept FloatingPoint = std::is_floating_point_v<T>; /*!< Type trait to check if T is a floating point type */

	template <typename T>
	concept RationalNumber = std::is

	template <typename T>
	concept String = std::is_same_v<std::string, std::remove_cvref_t<T>>; /*!< Type trait to check if T is a string type */
} // namespace Concepts

#endif