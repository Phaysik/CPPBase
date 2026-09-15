# ============================================================================
#  Coverage.cmake
#  Provides the Project::Coverage interface target used by the test target
#  when ENABLE_COVERAGE is ON.  Only meaningful for GCC/Clang.
# ============================================================================

add_library(project_coverage INTERFACE)
add_library(Project::Coverage ALIAS project_coverage)

if(MSVC)
    # MSVC uses a different instrumentation toolchain (VSInstr / /PROFILE);
    # leave the target empty so linking still works, and the user is expected
    # to enable coverage via Visual Studio's own tooling.
    return()
endif()

target_compile_options(project_coverage INTERFACE
    --coverage
    -fPIC
    -O0
    -g
    -fprofile-arcs
    -ftest-coverage
)

target_link_options(project_coverage INTERFACE --coverage)
