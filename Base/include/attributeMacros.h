/*! \file attributeMacros.h
	\brief Contains the attribute macros for creating portable C++ code
	\date --/--/----
	\version x.x.x
	\since x.x.x
	\author Matthew Moore
*/

#ifndef INCLUDE_ATTRIBUTEMACROS_H
#define INCLUDE_ATTRIBUTEMACROS_H

#ifdef __GNUC__
	/*! @def ATTR_GCC
		@brief Defined when compiling with GCC.
		@details This macro defined when the compiler predefined macro `__GNUC__` is present.
		Use it to enable GCC-specific attributes or workarounds.
		@note Do not assume exact GCC version from this macro; check `__GNUC__`/`__GNUC_MINOR__` when needed.
	*/
	#define ATTR_GCC
#endif

#ifdef __clang__
	/*! @def ATTR_CLANG
		@brief Defined when compiling with the Clang/LLVM compiler.
		@details This macro defined when the compiler predefined macro `__clang__` is present. Use it to conditionally enable Clang-specific
	   features or attributes.
		@note This macro is internal to the attribute macros helper and is not intended as a stable public API.
	*/
	#define ATTR_CLANG
#endif

#ifdef _MSC_VER
	/*! @def ATTR_MSVC
		@brief Defined when compiling with Microsoft Visual C++.
		@details This macro defined when `_MSC_VER` is defined by the compiler.
		Use it to guard MSVC-specific pragmas or attribute equivalents.
	*/
	#define ATTR_MSVC
#endif

// Portable attribute macro: use GCC/Clang attribute when available, otherwise empty.

#if defined(ATTR_GCC) || defined(ATTR_CLANG)
	#if __has_attribute(const)
		/*! @def ATTR_CONST
			@brief Portable macro for the compiler `const` attribute.
			@details Expands to `__attribute__((const))` on Clang and GCC, and to an empty token on other compilers.
			The `const` attribute conveys that a function has no side effects and its return value depends only on
			its arguments (not on global state). Use this macro to annotate library functions where this guarantee holds.
			@warning Misusing the attribute (annotating a function that reads global state or has side effects) can
			produce incorrect code generation.
			@example
			@code{.cpp}
			ATTR_CONST int pure_function(int x) { return x * 2; }
			@endcode
		*/
		#define ATTR_CONST __attribute__((const))
	#else
		#define ATTR_CONST
	#endif

	#if __has_attribute(pure)
		/*! @def ATTR_PURE
			@brief Portable macro for the C++ `[[pure]]` attribute.
			@details Expands to `[[pure]]` when supported by the compiler, otherwise to an empty token.
			The `[[pure]]` attribute indicates that a function has no side effects and its return value depends only on
			its arguments (not on global state). Use this macro to annotate library functions where this guarantee holds.
			@warning Misusing the attribute (annotating a function that reads global state or has side effects) can
			produce incorrect code generation.
			@example
			@code{.cpp}
			ATTR_PURE int compute_value(int x) { return x * 2; }
			@endcode
		*/
		#define ATTR_PURE __attribute__((pure))
	#else
		#define ATTR_PURE
	#endif

	#if __has_attribute(returns_nonnull)

		/*! @def ATTR_RETURNS_NONNULL
			@brief Portable macro for the compiler `returns_nonnull` attribute.
			@details Expands to `__attribute__((returns_nonnull))` on Clang and GCC, and to an empty token on other compilers. The
		   `returns_nonnull` attribute indicates that a function never returns a null pointer. Use this macro to annotate functions that
		   guarantee a non-null return value.
			@warning Misusing the attribute (annotating a function that may return null) can produce undefined behavior.
			@example
			@code{.cpp}
			ATTR_RETURNS_NONNULL int* get_value() { static int x = 42; return &x; }
			@endcode
		*/
		#define ATTR_RETURNS_NONNULL __attribute__((returns_nonnull))
	#else
		#define ATTR_RETURNS_NONNULL
	#endif

	#if __has_attribute(flag_enum)
		/*! @def ATTR_FLAG_ENUM
			@brief Portable macro for the compiler `flag_enum` attribute.
			@details Expands to `__attribute__((flag_enum))` on Clang and GCC, and to an empty token on other compilers. The `flag_enum`
		   attribute indicates that an enumeration is intended to be used as a bitmask (i.e., its values are powers of two). Use this
		   macro to annotate enum types that represent flags.
			@warning Misusing the attribute (annotating an enum that is not a bitmask) can produce incorrect code generation.
			@example
			@code{.cpp}
			enum class ATTR_FLAG_ENUM MyFlags {
				FlagA,
				FlagB,
				FlagC
			};
			@endcode
		*/
		#define ATTR_FLAG_ENUM __attribute__((flag_enum))
	#else
		#define ATTR_FLAG_ENUM
	#endif
#endif

#if defined(ATTR_CLANG)
	#if __has_attribute(enum_extensibility)
		/*! @def ATTR_ENUM_EXTENSIBILITY_OPEN
			@brief Portable macro for the Clang `enum_extensibility(open)` attribute.
			@details Expands to `__attribute__((enum_extensibility(open)))` on Clang, and to an empty token on other compilers. The
		   `enum_extensibility(open)` attribute indicates that an enumeration can take any values allowed by the standard and instructs
		   clang to be more lenient when issuing warnings.
			@warning Misusing the attribute (annotating an enum that is not intended to be extensible) can produce incorrect code
		   generation.
			@example
			@code{.cpp}
			enum ATTR_ENUM_EXTENSIBILITY_OPEN MyEnum {
				ValueA,
				ValueB,
				ValueC
			};

			enum ATTR_ENUM_EXTENSIBILITY_OPEN ATTR_FLAG_ENUM OpenFlagEnum {
				D0 = 1 << 0, D1 = 1 << 1
			};

			enum OpenEnum oe;
			enum OpenFlagEnum ofe;

			oe = B1;           // no warnings
			oe = 100;          // no warnings

			ofe = D0 | D1;     // no warnings
			ofe = D0 | D1 | 4; // no warnings
			@endcode
		*/
		#define ATTR_ENUM_EXTENSIBILITY_OPEN __attribute__((enum_extensibility(open)))
	#else
		#define ATTR_ENUM_EXTENSIBILITY_OPEN
	#endif

	#if __has_attribute(enum_extensibility)
		/*! @def ATTR_ENUM_EXTENSIBILITY_CLOSED
			@brief Portable macro for the Clang `enum_extensibility(closed)` attribute.
			@details Expands to `__attribute__((enum_extensibility(closed)))` on Clang, and to an empty token on other compilers. The
		   `enum_extensibility(closed)` attribute indicates that an enumeration takes a value that corresponds to one of the enumerators
		   listed in the enum definition, or when the enum is also annotated with @ref ATTR_FLAG_ENUM, a value that can be constructed
		   using values corresponding to the enumerators.
			@warning Misusing the attribute (annotating an enum that is not intended to be extensible) can produce incorrect code
		   generation.
			@example
			@code{.cpp}
			enum ATTR_ENUM_EXTENSIBILITY_CLOSED MyEnum {
				ValueA,
				ValueB,
				ValueC
			};

			enum ATTR_ENUM_EXTENSIBILITY_CLOSED ATTR_FLAG_ENUM OpenFlagEnum {
				C0 = 1 << 0, C1 = 1 << 1
			};

			enum ClosedEnum oe;
			enum ClosedFlagEnum cfe;

			oe = B1;           // no warnings
			oe = 100;          // no warnings

			cfe = C0 | C1;     // no warnings
			cfe = C0 | C1 | 4; // warning issued
			@endcode
		*/
		#define ATTR_ENUM_EXTENSIBILITY_CLOSED __attribute__((enum_extensibility(closed)))
	#else
		#define ATTR_ENUM_EXTENSIBILITY_CLOSED
	#endif

	#if __has_attribute(using_if_exists)
		/*! @def ATTR_USING_IF_EXISTS
			@brief Portable macro for the Clang `using_if_exists` attribute.
			@details Expands to `__attribute__((using_if_exists))` on Clang, and to an empty token on other compilers. The `using_if_exists`
		   attribute allows a using declaration to refer to a name that may not exist without causing an error. Use this macro to
		   conditionally import names that may not be present in all versions of a library or API.
			@warning Misusing the attribute (annotating a using declaration that must refer to an existing name) can produce incorrect code
		   generation.
			@example
			@code{.cpp}
			namespace LibraryV1 {
				struct TypeA {};
			}

			namespace LibraryV2 {
				struct TypeA {};
				struct TypeB {};
			}

			using namespace LibraryV1;
			using namespace LibraryV2;

			using TypeB ATTR_USING_IF_EXISTS; // no error if TypeB doesn't exist

			TypeA a; // OK
			TypeB b; // OK if TypeB exists, otherwise compile error
			@endcode
		*/
		#define ATTR_USING_IF_EXISTS __attribute__((using_if_exists))
	#else
		#define ATTR_USING_IF_EXISTS
	#endif

	#if __has_cpp_attribute(clang::no_specializations)
		/*! @def ATTR_NO_SPECIALIZATIONS
			@brief Portable macro for the Clang `[[clang::no_specializations]]` attribute.
			@details Expands to `[[clang::no_specializations]]` on Clang, and to an empty token on other compilers. The
		   `[[clang::no_specializations]]` attribute indicates that a function template should not be instantiated with any template
		   arguments. Use this macro to annotate function templates that are intended to be used only with explicit specializations.
			@warning Misusing the attribute (annotating a function template that is intended to be instantiated) can produce incorrect code
		   generation.
			@example
			@code{.cpp}
			template <typename T>
			ATTR_NO_SPECIALIZATIONS void foo(T) {
				// implementation
			}

			template <>
			void foo<int>(int) {
				// specialization for int
			}

			int main() {
				foo(42); // error: no matching function for call to 'foo'
				return 0;
			}
			@endcode
		*/
		#define ATTR_NO_SPECIALIZATIONS [[clang::no_specializations]]
	#else
		#define ATTR_NO_SPECIALIZATIONS
	#endif

	#if __has_cpp_attribute(clang::preferred_type)
		/*! @def ATTR_PREFERRED_TYPE
			@brief Portable macro for the Clang `[[clang::preferred_type(T)]]` attribute.
			@details Expands to `[[clang::preferred_type(T)]]` on Clang, and to an empty token on other compilers. The
		   `[[clang::preferred_type(T)]]` attribute allows adjusting the tpye of a bit-field in debug information. This can be helpful when
		   a bit-field is intended to store an enumeration value, but has to be specified as having the enumeration's underlying type.
			@warning Misusing the attribute (annotating a function template that is intended to be instantiated) can produce incorrect code
		   generation.
			@example
			@code{.cpp}
			enum class Colors {
				Red,
				Green,
				Blue
			};

			struct S {
				[[clang::preferred_type(Colors)]] unsigned ColorVal : 2;
				[[clang::preferred_type(bool)]] unsigned UseAlternateColorSpace : 1;
			} s = { Green, false };
			@endcode
		*/
		#define ATTR_PREFERRED_TYPE(param) [[clang::preferred_type(param)]]
	#else
		#define ATTR_PREFERRED_TYPE(param)
	#endif
#else
	#define ATTR_ENUM_EXTENSIBILITY_OPEN
	#define ATTR_ENUM_EXTENSIBILITY_CLOSED
	#define ATTR_USING_IF_EXISTS
	#define ATTR_NO_SPECIALIZATIONS
	#define ATTR_PREFERRED_TYPE(param)
#endif

#if __has_cpp_attribute(nodiscard)
	/*! @def ATTR_NODISCARD
		@brief Portable macro for the C++ `[[nodiscard]]` attribute.
		@details Expands to `[[nodiscard]]` when supported by the compiler, otherwise to an empty token.
		The `[[nodiscard]]` attribute indicates that the return value of a function should not be ignored.
		Use this macro to annotate functions where ignoring the return value is likely a bug.
		@example
		@code{.cpp}
		ATTR_NODISCARD int compute_value() { return 42; }
		@endcode
	*/
	#define ATTR_NODISCARD [[nodiscard]]
#else
	#define ATTR_NODISCARD
#endif

#if __has_cpp_attribute(maybe_unused)
	/*! @def ATTR_MAYBE_UNUSED
		@brief Portable macro for the C++ `[[maybe_unused]]` attribute.
		@details Expands to `[[maybe_unused]]` when supported by the compiler, otherwise to an empty token.
		The `[[maybe_unused]]` attribute indicates that the a parameter may be unused.
		@example
		@code{.cpp}
		int compute_value(ATTR_MAYBE_UNUSED int number) { return number; }
		@endcode
	*/
	#define ATTR_MAYBE_UNUSED [[maybe_unused]]
#else
	#define ATTR_MAYBE_UNUSED
#endif

#if __has_cpp_attribute(deprecated)
	/*! @def ATTR_DEPRECATED
		@brief Portable macro for the C++ `[[deprecated]]` attribute.
		@details Expands to `[[deprecated]]` when supported by the compiler, otherwise to an empty token.
		The `[[deprecated]]` attribute indicates that a function is deprecated and should not be used.
		@example
		@code{.cpp}
		ATTR_DEPRECATED int compute_value(int number) { return number; }
		@endcode
	*/
	#define ATTR_DEPRECATED [[deprecated]]
#else
	#define ATTR_DEPRECATED
#endif

#if __has_cpp_attribute(noreturn)
	/*! @def ATTR_NORETURN
		@brief Portable macro for the C++ `[[noreturn]]` attribute.
		@details Expands to `[[noreturn]]` when supported by the compiler, otherwise to an empty token.
		The `[[noreturn]]` attribute indicates that a function will not return control flow to the calling function after it
	   finishes.
		@example
		@code{.cpp}
		ATTR_NORETURN int compute_value(int number) { return number; }
		@endcode
	*/
	#define ATTR_NORETURN [[noreturn]]
#else
	#define ATTR_NORETURN
#endif

#if __has_cpp_attribute(fallthrough)
	/*! @def ATTR_FALLTHROUGH
		@brief Portable macro for the C++ `[[fallthrough]]` attribute.
		@details Expands to `[[fallthrough]]` when supported by the compiler, otherwise to an empty token.
		The `[[fallthrough]]` attribute indicates that the fall through from the previous case label is intentional and should not
	   be diagnosed.
		@example
		@code{.cpp}
		int compute_value(int number) {
			switch (number) {
				case 1:
				case 2:
					foo();
					ATTR_FALLTHROUGH;
				case 3: // No warning on fallthrough
					bar();
				default:
					break;
			}
		}
		@endcode
	*/
	#define ATTR_FALLTHROUGH [[fallthrough]]
#else
	#define ATTR_FALLTHROUGH
#endif

#if __has_cpp_attribute(likely)
	/*! @def ATTR_LIKELY
		@brief Portable macro for the C++ `[[likely]]` attribute.
		@details Expands to `[[likely]]` when supported by the compiler, otherwise to an empty token.
		The `[[likely]]` attribute indicates that the a code path is more likely to be executed than the others.
		@example
		@code{.cpp}
		constexpr double pow(doulbe x, uint64_t n) noexcept {
			if (n > 0) ATTR_LIKELY
				return x * pow(x, n - 1);
			else ATTR_UNLIKELY
				return 1;
		}
		@endcode
	*/
	#define ATTR_LIKELY [[likely]]
#else
	#define ATTR_LIKELY
#endif

#if __has_cpp_attribute(unlikely)
	/*! @def ATTR_UNLIKELY
		@brief Portable macro for the C++ `[[unlikely]]` attribute.
		@details Expands to `[[unlikely]]` when supported by the compiler, otherwise to an empty token.
		The `[[unlikely]]` attribute indicates that the a code path is more unlikely to be executed than the others.
		@example
		@code{.cpp}
		constexpr double pow(doulbe x, uint64_t n) noexcept {
			if (n > 0) ATTR_UNLIKELY
				return x * pow(x, n - 1);
			else ATTR_UNUNLIKELY
				return 1;
		}
		@endcode
	*/
	#define ATTR_UNLIKELY [[unlikely]]
#else
	#define ATTR_UNLIKELY
#endif

#endif
