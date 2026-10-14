# ============================================================================
#  Reflection.cmake
#
#  Provides C++26 static reflection support via Project::Reflection.
#
#  GCC 16.1+ requires -freflection to enable reflection; the flag is only
#  valid together with -std=c++26.  Other compilers (Clang, MSVC) do not
#  implement reflection yet, so the target is a no-op there.
#
#  Controlled by the ENABLE_REFLECTION cache option.  If the compiler does
#  not accept -freflection the flag is silently dropped.
# ============================================================================

add_library(project_reflection INTERFACE)
add_library(Project::Reflection ALIAS project_reflection)

if(NOT ENABLE_REFLECTION)
    return()
endif()

include(CheckCXXCompilerFlag)

# Probe without -Werror so a warning on the empty test TU does not cause a
# perfectly valid flag to be dropped.
check_cxx_compiler_flag("-freflection" _HAS_FREFLECTION)

if(_HAS_FREFLECTION)
    target_compile_options(project_reflection INTERFACE -freflection)
endif()
