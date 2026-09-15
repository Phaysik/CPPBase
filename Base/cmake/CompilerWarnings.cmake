# ============================================================================
#  CompilerWarnings.cmake
#
#  Provides two interface targets:
#    Project::Warnings      — full warning suite; link on every target.
#    Project::ReleaseFlags  — release-only hardening; link AFTER Warnings.
#
#  WHY THE SPLIT AND THE ORDERING
#  ------------------------------
#  -fhardened implicitly sets -fno-delete-null-pointer-checks.  The upstream
#  Makefile emitted flags in this order:
#
#      ... ${WARNINGS} ${RELEASE_WARNINGS} ...
#
#  which, after expansion, put -fhardened *after* everything else, so
#  -fno-delete-null-pointer-checks won.  A naive CMake port that puts
#  -fhardened in target_compile_options emits it *before* the interface
#  warnings, so -fdelete-null-pointer-checks (if present) or the compiler's
#  own default would win — a real codegen difference that changes what the
#  optimiser sees and, on GCC 16.1, exposes several RTL-pass ICEs.
#
#  By keeping -fhardened in a *second* interface target that is linked after
#  Project::Warnings, the emitted order matches the Makefile exactly.
# ============================================================================

# ---------------------------------------------------------------------------
#  Interface targets
# ---------------------------------------------------------------------------
add_library(project_warnings INTERFACE)
add_library(Project::Warnings ALIAS project_warnings)

add_library(project_release_flags INTERFACE)
add_library(Project::ReleaseFlags ALIAS project_release_flags)

# ---------------------------------------------------------------------------
#  MSVC
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4
        /permissive-
        /utf-8
        /Zc:__cplusplus
        /Zc:preprocessor
        /EHsc
        /MP
        /bigobj
    )
    if(WARNINGS_AS_ERRORS)
        target_compile_options(project_warnings INTERFACE /WX)
    endif()
    # No release hardening on MSVC.
    return()
endif()

# ---------------------------------------------------------------------------
#  GCC / Clang
# ---------------------------------------------------------------------------
include(CheckCXXCompilerFlag)

# Probe each flag WITHOUT -Werror.  If -Werror is set during the probe, a
# flag that happens to fire on the empty test TU (-Weffc++, -Wformat-security,
# -Wformat-nonliteral, -Wformat-y2k, ...) will be silently dropped even though
# it is perfectly valid for real code.  Any flag that genuinely does not exist
# causes a hard error in the probe TU and is dropped correctly.
function(_add_warning_if_supported flag)
    string(MAKE_C_IDENTIFIER "WARN_SUPPORTS${flag}" _var)
    if(NOT DEFINED ${_var})
        check_cxx_compiler_flag("${flag}" ${_var})
    endif()
    if(${_var})
        target_compile_options(project_warnings INTERFACE "${flag}")
    endif()
endfunction()

# NOTE: -fdelete-null-pointer-checks is deliberately NOT in this list.  It is
# the exact flag that -fhardened overrides via -fno-delete-null-pointer-checks;
# putting it here would fight -fhardened depending on ordering.
set(_all_warnings
    # language conformance / strictness
    -pedantic
    -pedantic-errors
    -fstrict-aliasing
    -fimplicit-constexpr

    # baseline
    -Wall
    -Wextra
    -Weffc++

    # type & ABI safety
    -Wcast-align
    -Wcast-qual
    -Wctor-dtor-privacy
    -Wzero-as-null-pointer-constant
    -Wold-style-cast
    -Wuseless-cast
    -Wconversion
    -Wsign-conversion
    -Wsign-promo
    -Wfloat-equal
    -Wdouble-promotion
    -Wenum-conversion

    # printf-family format checking
    -Wformat=2
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-signedness
    -Wformat-truncation=2
    -Wformat-y2k
    -Wmissing-format-attribute

    # classes / OOP
    -Woverloaded-virtual
    -Wnoexcept
    -Wsuggest-final-types
    -Wsuggest-final-methods
    -Wsuggest-override

    # declarations / misc
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wmissing-noreturn
    -Wredundant-decls
    -Wshadow
    -Wundef
    -Wunused
    -Wuninitialized
    -Wpacked
    -Wunused-macros
    -Wunused-const-variable=2
    -Wvariadic-macros
    -Wextra-semi
    -Wdisabled-optimization
    -Wnull-dereference
    -Wstrict-aliasing
    -Wswitch-default
    -Wswitch-enum

    # GCC / Clang specific — silently dropped when the compiler rejects them
    -Wlogical-op
    -Wduplicated-branches
    -Wduplicated-cond
    -Wtrampolines
    -Wbidi-chars=any
    -Wstrict-null-sentinel
    -Wunsafe-loop-optimizations
    -Wno-changes-meaning
    -Winvalid-constexpr
    -Wnrvo
    -Wcomma-subscript
    -Wdangling-reference
    -Wsuggest-attribute=pure
    -Wsuggest-attribute=const
    -Wsuggest-attribute=malloc
    -Wsuggest-attribute=cold
    -Wsuggest-attribute=format
    -Wsuggest-attribute=returns_nonnull
    -Wno-missing-requires
    -Wno-missing-template-keyword
)

foreach(_w IN LISTS _all_warnings)
    _add_warning_if_supported("${_w}")
endforeach()

if(WARNINGS_AS_ERRORS)
    target_compile_options(project_warnings INTERFACE -Werror)
endif()

# ---------------------------------------------------------------------------
#  Release hardening — linked after Project::Warnings so -fhardened's
#  -fno-delete-null-pointer-checks wins, matching the upstream Makefile.
# ---------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    check_cxx_compiler_flag("-fhardened" _HAS_FHARDENED)
    if(_HAS_FHARDENED)
        target_compile_options(project_release_flags INTERFACE -fhardened)
        target_link_options(project_release_flags INTERFACE -fhardened)
    endif()

    check_cxx_compiler_flag("-Whardened" _HAS_WHARDENED)
    if(_HAS_WHARDENED)
        target_compile_options(project_release_flags INTERFACE -Whardened)
    endif()
endif()
