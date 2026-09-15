add_library(project_sanitizers INTERFACE)
add_library(Project::Sanitizers ALIAS project_sanitizers)

if(MSVC)
    target_compile_options(project_sanitizers INTERFACE /fsanitize=address)
    target_link_options(   project_sanitizers INTERFACE
        /fsanitize=address
        /INCREMENTAL:NO
    )
    return()
endif()

include(CheckCXXCompilerFlag)

function(_add_sanitizer_if_supported flag)
    set(_probe_flag "${flag}")
    set(CMAKE_REQUIRED_LINK_OPTIONS "${flag}")
    if(flag STREQUAL "-fsanitize=pointer-compare" OR
       flag STREQUAL "-fsanitize=pointer-subtract")
        set(_probe_flag "-fsanitize=address ${flag}")
        list(PREPEND CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
    endif()

    string(MAKE_C_IDENTIFIER "SAN_LINK_SUPPORTS${flag}" _var)
    check_cxx_compiler_flag("${_probe_flag}" ${_var})
    if(${_var})
        target_compile_options(project_sanitizers INTERFACE "${flag}")
        target_link_options(   project_sanitizers INTERFACE "${flag}")
    endif()
endfunction()

set(_sanitizers
    -fsanitize=address
    -fsanitize=pointer-compare
    -fsanitize=pointer-subtract
    -fsanitize=leak
    -fsanitize=undefined
    -fsanitize=shift
    -fsanitize=shift-exponent
    -fsanitize=shift-base
    -fsanitize=integer-divide-by-zero
    -fsanitize=unreachable
    -fsanitize=vla-bound
    -fsanitize=null
    -fsanitize=return
    -fsanitize=signed-integer-overflow
    -fsanitize=bounds
    -fsanitize=bounds-strict
    -fsanitize=alignment
    -fsanitize=float-divide-by-zero
    -fsanitize=float-cast-overflow
    -fsanitize=nonnull-attribute
    -fsanitize=returns-nonnull-attribute
    -fsanitize=bool
    -fsanitize=enum
    -fsanitize=vptr
    -fsanitize=pointer-overflow
    -fsanitize=builtin
    -fsanitize-address-use-after-scope
    -fno-omit-frame-pointer)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND _sanitizers -fsanitize=object-size)
endif()

foreach(_s IN LISTS _sanitizers)
    _add_sanitizer_if_supported("${_s}")
endforeach()
