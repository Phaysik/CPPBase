/*! @file defaultRandomSource.h
	@brief Provides a default implementation of a random number source using the Random utility.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#ifndef INCLUDE_UTILITY_RANDOM_DEFAULT_RANDOM_SOURCE_H
#define INCLUDE_UTILITY_RANDOM_DEFAULT_RANDOM_SOURCE_H

#include "Core/concepts.h"

#include "random.h"
#include "randomSourceInterface.h"

namespace Project::Utility::Random
{
	using Project::Core::FloatingPoint;
	using Project::Core::Integral;

	/*! @struct DefaultRandomSource defaultRandomSource.h "Utility/Random/defaultRandomSource.h"
		@brief Implements @ref IRandomSource using the shared random number generator in @ref Random.
		@details Delegates generation to @ref Random::get, using a uniform integer distribution for integral types
			and a uniform real distribution for floating-point types.
		@tparam T The generated numeric type, satisfying @ref Project::Core::Integral or @ref Project::Core::FloatingPoint.
		@note Instances share generator state; seeding through @ref Random::setSeed affects every instance.
		@warning Not thread-safe. The caller must synchronize concurrent access to the shared generator, including across instances.
		@date --/--/----
		@version x.x.x
		@since x.x.x
	*/
	template <typename T>
		requires Integral<T> || FloatingPoint<T>
	struct DefaultRandomSource final : public IRandomSource<T>
	{
		public:
			/*! @brief Generates a uniformly distributed value by delegating to @ref Random::get.
				@pre min <= max. For floating-point types, the bounds are finite and max - min does not exceed the largest representable
			   value of T.
				@param[in] min The inclusive lower bound.
				@param[in] max The inclusive upper bound for integral types, or exclusive upper bound for floating-point types.
				@return A random value in [min, max] for integral types or [min, max) for floating-point types.
					Returns min when min equals max.
				@note Advances the shared generator state. Bounds are not validated; callers must satisfy the distribution's preconditions.
				@date --/--/----
				@version x.x.x
				@since x.x.x
			*/
			T operator()(const T min, const T max) override
			{
				return Random::get<T>(min, max);
			}
	};

	/*! @brief Returns the shared default random source for the requested numeric type.
		@tparam T The generated numeric type, satisfying @ref Project::Core::Integral or @ref Project::Core::FloatingPoint.
		@return A non-owning reference to a function-local static @ref DefaultRandomSource exposed through @ref IRandomSource.
			The reference remains valid until the static object's destruction during program termination; callers must not delete it.
		@note The source is initialized on first use, and subsequent calls for the same T return the same object.
			Sources for different types still use the shared generator in @ref Random.
		@note Initialization is thread-safe. Generating values is not thread-safe; the caller must synchronize concurrent access
			to the shared generator, including access through sources for different types.
		@date --/--/----
		@version x.x.x
		@since x.x.x
	*/
	template <typename T>
		requires Integral<T> || FloatingPoint<T>
	IRandomSource<T> &getDefaultRandomSource()
	{
		static DefaultRandomSource<T> source{}; // LCOV_EXCL_BR
		return source;
	}
} // namespace Project::Utility::Random

#endif
