# ============================================================================
#  Contracts.cmake
#
#  Provides GCC-only C++26 contract support.
#
#  Two interface targets:
#    Project::ContractsDebug   -> quick_enforce semantics
#    Project::ContractsRelease -> observe semantics
#
# ============================================================================

add_library(project_contracts_debug INTERFACE)
add_library(Project::ContractsDebug ALIAS project_contracts_debug)

add_library(project_contracts_release INTERFACE)
add_library(Project::ContractsRelease ALIAS project_contracts_release)

if (ENABLE_CONTRACTS)
    include(CheckCXXCompilerFlag)

    check_cxx_compiler_flag("-fcontracts" _HAS_FCONTRACTS)

    if(_HAS_FCONTRACTS)
        # Common flag for both configurations
        target_compile_options(project_contracts_debug   INTERFACE -fcontracts)
        target_compile_options(project_contracts_release INTERFACE -fcontracts)

        # Debug: quick_enforce
        check_cxx_compiler_flag(
            "-fcontract-evaluation-semantic=quick_enforce"
            _HAS_CONTRACTS_QUICK_ENFORCE
        )
        if(_HAS_CONTRACTS_QUICK_ENFORCE)
            target_compile_options(project_contracts_debug INTERFACE
                -fcontract-evaluation-semantic=quick_enforce
            )
        endif()

        # Release: observe
        check_cxx_compiler_flag(
            "-fcontract-evaluation-semantic=observe"
            _HAS_CONTRACTS_OBSERVE
        )
        if(_HAS_CONTRACTS_OBSERVE)
            target_compile_options(project_contracts_release INTERFACE
                -fcontract-evaluation-semantic=observe
            )
        endif()
    endif()
endif()
