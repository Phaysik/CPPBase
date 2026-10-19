/*! @file callableRandomSource.h
	@brief Provides a random source that delegates generation to a callable function.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#ifndef INCLUDE_UTILITY_RANDOM_CALLABLE_RANDOM_SOURCE_H
#define INCLUDE_UTILITY_RANDOM_CALLABLE_RANDOM_SOURCE_H

#include <functional>

#include "Core/concepts.h"

#include "randomSourceInterface.h"

namespace Project::Utility::Random
{
	using Project::Core::FloatingPoint;
	using Project::Core::Integral;
	using Project::Core::RandomFn;

	/*! @class CallableRandomSource callableRandomSource.h "Utility/Random/callableRandomSource.h"
		@brief Adapts a user-supplied callable to the @ref IRandomSource interface.
		@details Delegates each generation request to the stored callable without validating bounds or results.
			The callable defines range semantics, distribution, and any generator state changes.
		@tparam T The generated numeric type, satisfying @ref Project::Core::Integral or @ref Project::Core::FloatingPoint.
		@tparam Func The callable type, satisfying @ref Project::Core::RandomFn for T; invocation on an lvalue accepts two T arguments
	   and returns T.
		@note Resources referenced by the callable must remain valid for every invocation.
			Thread-safety depends on the callable and any shared state it accesses; callers must provide synchronization where required.
		@date --/--/----
		@version x.x.x
		@since x.x.x
	*/
	template <typename T, RandomFn<T> Func>
		requires Integral<T> || FloatingPoint<T>
	class CallableRandomSource final : public IRandomSource<T>
	{
		public:
			/*! @brief Initializes the adapter by moving the supplied callable into storage.
				@param[in] func The callable to store and invoke for subsequent generation requests.
				@note Exceptions from callable construction propagate to the caller.
					Storing the callable does not extend the lifetime of objects it references.
				@date --/--/----
				@version x.x.x
				@since x.x.x
			*/
			explicit CallableRandomSource(Func func) : mFunc(std::move(func)) {}

			/*! @brief Invokes the stored callable with the supplied bounds.
				@pre The bounds satisfy the stored callable's preconditions.
				@param[in] min The lower bound, with inclusion determined by the callable.
				@param[in] max The upper bound, with inclusion determined by the callable.
				@return The value returned by the callable, unchanged.
				@note Invokes the callable as an lvalue and may modify its state.
					Exceptions from invocation propagate to the caller; no additional exception guarantee is imposed.
				@date --/--/----
				@version x.x.x
				@since x.x.x
			*/
			T operator()(const T min, const T max) override
			{
				return std::invoke(mFunc, min, max);
			}

		private:
			/*! @brief Stores the callable used to handle generation requests. */
			Func mFunc;
	};
} // namespace Project::Utility::Random

#endif
