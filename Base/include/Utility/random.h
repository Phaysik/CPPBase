/*! @file random.h
	@brief Contains the function declarations for creating a random number generator
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#ifndef INCLUDE_RANDOM_H
#define INCLUDE_RANDOM_H

#include <chrono>
#include <concepts>
#include <random>
#include <utility>

#include "Core/attributeMacros.h"
#include "Core/cconcepts.h" // for Integral
#include "Core/typedefs.h"

/*! @namespace PocketCore::Utility Holds any useful functionality that doesn't fit anywhere else
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/
namespace PocketCore::Utility
{
	using PocketCore::Core::ul;

	/*! @class Random random.h "include/random.h"
		@brief Class for creating a random number generator
		@since x.x.x
		@version x.x.x
		@author Matthew Moore
	*/
	class Random
	{
		public:
			/*! @brief Gets a random number in the range [min, max] wtih a templated return type in case you need to cast the uniform
			   distribution result to a different type
				@tparam T The type to cast the uniform distribution result to
				@param[in] min The minimum value (inclusive)
				@param[in] max The maximum value (inclusive)
				@retval T The typecasted random number
				@since x.x.x
				@version x.x.x
				@author Matthew Moore
			*/
			template <PocketCore::Core::Integral T>
			ATTR_NODISCARD static T get(const T min, const T max) noexcept
			{
				return std::uniform_int_distribution<T>{min, max}(mTwister);
			}

			/*! @brief Gets a random number in the range [min, max) wtih a templated return type in case you need to cast the real
			   distribution result to a different type
				@tparam T The type to cast the real distribution result to
				@param[in] min The minimum value (inclusive)
				@param[in] max The maximum value (exclusive)
				@retval T The typecasted random number
				@since x.x.x
				@version x.x.x
				@author Matthew Moore
			*/
			template <PocketCore::Core::FloatingPoint T>
			ATTR_NODISCARD static T get(const T min, const T max) noexcept
			{
				return std::uniform_real_distribution<T>{min, max}(mTwister);
			}

			/*! @brief Gets #mTwister
				@retval std::mt19937 The global random number generator
				@since x.x.x
				@version x.x.x
				@author Matthew Moore
			*/
			ATTR_NODISCARD static std::mt19937 &getTwister() noexcept
			{
				return mTwister;
			}

			/*! @brief Sets the seed for #mTwister
				@param[in] seed The seed value
				@since x.x.x
				@version x.x.x
				@author Matthew Moore
			*/
			static void setSeed(const ul seed) noexcept
			{
				mTwister.seed(seed);
			}

			/*! @brief Gets a seed for the random number generator based on a function and desired result
				@tparam Func The function type
				@tparam DesiredValue The desired result type
				@param[in] func The function to invoke
				@param[in] result The desired result
				@retval std::size_t The seed value
				@since x.x.x
				@version x.x.x
				@author Matthew Moore
			*/
			template <typename Func, typename DesiredValue>
				requires PocketCore::Core::InvocableNoArgs<Func> && std::same_as<std::invoke_result_t<Func>, DesiredValue>
			ATTR_NODISCARD static ul findSeed(Func &&func, const DesiredValue &result) noexcept
			{
				Func &&forwardedFunc{std::forward<Func>(func)};
				std::size_t seedIncrement{0};

				while (result != forwardedFunc())
				{
					mTwister.seed(seedIncrement++);
				}

				return --seedIncrement;
			}

		private:
			/*! @brief Creates the global random number generator
				@retval std::mt19937 The global random number generator
				@since x.x.x
				@version x.x.x
				@author Matthew Moore
			*/
			ATTR_NODISCARD static std::mt19937 generate() noexcept
			{
				std::random_device randomDevice{};

				// Create seed_seq with high-res clock and 7 random numbers from std::random_device
				std::seed_seq seedSequence{
					static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
					randomDevice(),
					randomDevice(),
					randomDevice(),
					randomDevice(),
					randomDevice(),
					randomDevice(),
					randomDevice(),
				};

				return std::mt19937{seedSequence};
			}

			static inline std::mt19937 mTwister{generate()}; /*!< The global random number generator */
	};
} // namespace PocketCore::Utility

#endif
