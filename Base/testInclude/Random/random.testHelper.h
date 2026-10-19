/*! @file random.testHelper.h
	@brief Header file for running tests for Randomness.
	@date --/--/----
	@since x.x.x
	@version x.x.x
	@author Matthew Moore
*/

#ifndef TEST_INCLUDE_RANDOM_TEST_HELPER_H
#define TEST_INCLUDE_RANDOM_TEST_HELPER_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <vector>

#include "Core/attributeMacros.h"

namespace Project::Testing
{
	template <typename TypeValue>
	struct FixedRandom
	{
		public:
			ATTR_NODISCARD TypeValue operator()(TypeValue /* min */, TypeValue /* min */) const noexcept
			{
				return value;
			}

		public:
			// NOLINTBEGIN(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
			TypeValue value;
			// NOLINTEND(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
	};

	template <typename TypeValue>
	struct SequenceRandom
	{
		public:
			SequenceRandom(std::initializer_list<TypeValue> init) : values(init) {}

			TypeValue operator()(TypeValue /*min*/, TypeValue /*max*/)
			{
				if (index >= values.size())
				{
					throw std::out_of_range("SequenceRandom exhausted");
				}

				return values[index++];
			}

			ATTR_NODISCARD std::size_t calls() const noexcept
			{
				return index;
			}

		public:
			// NOLINTBEGIN(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
			std::vector<TypeValue> values;
			std::size_t index{0};
			// NOLINTEND(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
	};
} // namespace Project::Testing

#endif
