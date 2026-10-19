/*! @file randomSourceInterface.h
	@brief Defines the interface for a random number source. Contains the interface for random number sources.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#ifndef INCLUDE_UTILITY_RANDOM_RANDOM_SOURCE_INTERFACE_H
#define INCLUDE_UTILITY_RANDOM_RANDOM_SOURCE_INTERFACE_H

#include "Core/concepts.h"

namespace Project::Utility::Random
{
	using Project::Core::FloatingPoint;
	using Project::Core::Integral;

	/*! @class IRandomSource randomSourceInterface.h "Utility/Random/randomSourceInterface.h"
		@brief Provides a polymorphic interface for generating random numeric values within supplied bounds.
		@details Derived classes implement the generation strategy through @ref operator().
			Instances cannot be copied or moved through this interface.
		@tparam T The generated numeric type, satisfying @ref Project::Core::Integral or @ref Project::Core::FloatingPoint.
		@note Implementations define endpoint inclusion, valid bounds, error handling, and thread-safety guarantees.
		@date --/--/----
		@since x.x.x
		@version x.x.x
	*/
	template <typename T>
		requires Integral<T> || FloatingPoint<T>
	struct IRandomSource
	{
		public:
			/*! @brief Default-constructs the interface subobject for a derived random source.
				@since x.x.x
				@version x.x.x
			 */
			IRandomSource() = default;

			/*! @brief Disables copy construction of the interface.
				@since x.x.x
				@version x.x.x
			 */
			IRandomSource(const IRandomSource &) = delete;

			/*! @brief Disables move construction of the interface.
				@since x.x.x
				@version x.x.x
			 */
			IRandomSource(IRandomSource &&) = delete;

			/*! @brief Disables copy assignment of the interface.
				@since x.x.x
				@version x.x.x
			 */
			IRandomSource &operator=(const IRandomSource &) = delete;

			/*! @brief Disables move assignment of the interface.
				@since x.x.x
				@version x.x.x
			 */
			IRandomSource &operator=(IRandomSource &&) = delete;

			/*! @brief Allows derived random sources to be destroyed through an interface pointer.
				@since x.x.x
				@version x.x.x
			 */
			virtual ~IRandomSource() = default; // LCOV_EXCL_LINE

			/*! @brief Generates a random value using the supplied bounds.
				@param[in] min The lower bound, subject to the implementation's range contract.
				@param[in] max The upper bound, subject to the implementation's range contract.
				@return A generated value of type T within the implementation-defined interval.
				@note Implementations specify endpoint inclusion, preconditions, exceptions, and any generator state changes.
				@date --/--/----
				@version x.x.x
				@since x.x.x
			*/
			virtual T operator()(const T min, const T max) = 0;
	};
} // namespace Project::Utility::Random

#endif
