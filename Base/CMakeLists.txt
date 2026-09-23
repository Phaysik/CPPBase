# ============================================================================
#  CMakeLists.txt — CMake 4.4+ / Ninja
#  Supports GCC, Clang, MSVC on Linux, macOS, Windows.
# ============================================================================
cmake_minimum_required(VERSION 4.4)

project(CPPBase
    VERSION     0.1.0
    LANGUAGES   CXX
    DESCRIPTION "Cross-platform C++26 project (Makefile -> CMake + Ninja)"
)

# ----------------------------------------------------------------------------
#  Global configuration
# ----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD            26)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)
set(CMAKE_CXX_EXTENSIONS          OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC AND CMAKE_CXX_STANDARD EQUAL 26)
    set(CMAKE_CXX_STANDARD_REQUIRED OFF)
else()
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# CMake 4.4 auto-enables C++20 module scanning when CMAKE_CXX_STANDARD >= 20.
# That injects -fmodules-ts / -fmodule-mapper= / -x c++ into every compile,
# which triggers ICEs on GCC 16.1.  The project does not use modules.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF CACHE BOOL "Disable C++20 module scanning" FORCE)

# Ninja is single-config; ensure a build type is always defined.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                 Debug Release RelWithDebInfo MinSizeRel)
endif()

# We drive optimisation / debug flags per-target, so blank the defaults.
foreach(_cfg RELEASE DEBUG RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_CXX_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
endforeach()

# ----------------------------------------------------------------------------
#  Options
# ----------------------------------------------------------------------------
option(BUILD_TESTS        "Build the test target"                          ON)
option(BUILD_BENCHMARKS   "Build the benchmark target"                     OFF)
option(BUILD_TRACY        "Build the Tracy profiling target"               OFF)
option(WARNINGS_AS_ERRORS "Treat warnings as errors"                       ON)
option(ENABLE_SANITIZERS  "Enable ASan/UBSan on the dev target"            ON)
option(ENABLE_COVERAGE    "Enable coverage instrumentation on tests"       OFF)
option(ENABLE_PROFILING   "Enable gprof (-pg) on the dev target"           OFF)

set(TEST_OPTION "full" CACHE STRING "Which tests to build")
set_property(CACHE TEST_OPTION PROPERTY STRINGS full integrations mocks unit)

set(TEST_STANDARD "catch2" CACHE STRING "Test framework")
set_property(CACHE TEST_STANDARD PROPERTY STRINGS catch2 googletest)

# ----------------------------------------------------------------------------
#  Directory layout
# ----------------------------------------------------------------------------
set(SRC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(RES_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(TEST_DIR       "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(TEST_INC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/testInclude")
set(BENCH_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")
set(TEST_INTEG_DIR "${TEST_DIR}/integrations")
set(TEST_MOCKS_DIR "${TEST_DIR}/mocks")
set(TEST_UNIT_DIR  "${TEST_DIR}/unit")
set(TEST_MAIN_DIR  "${TEST_DIR}/main")

# ----------------------------------------------------------------------------
#  Source collection
# ----------------------------------------------------------------------------
set(EXCLUDED_FILES   "testMain.cpp" "configCat.cpp")
set(EXCLUDED_FOLDERS "tracy")

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")
foreach(_f IN LISTS EXCLUDED_FILES)
    list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/${_f}$")
endforeach()
foreach(_d IN LISTS EXCLUDED_FOLDERS)
    list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/${_d}/")
endforeach()

set(PROJECT_SUPPORT_SOURCES ${PROJECT_SOURCES})
list(REMOVE_ITEM PROJECT_SUPPORT_SOURCES "${SRC_DIR}/main.cpp")

# ----------------------------------------------------------------------------
#  Modules
# ----------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)
include(Coverage)

# ----------------------------------------------------------------------------
#  Helper functions
# ----------------------------------------------------------------------------
# NOTE: release hardening (-fhardened -Whardened) lives in Project::ReleaseFlags
# and is linked *after* Project::Warnings by every release target.  Do NOT add
# it here — target_compile_options flags are emitted BEFORE interface flags,
# which would reverse the ordering relative to the upstream Makefile.
function(apply_release_flags tgt)
    if(MSVC)
        target_compile_options(${tgt} PRIVATE /O2)
    else()
        target_compile_options(${tgt} PRIVATE -O3)
    endif()
    target_compile_definitions(${tgt} PRIVATE NDEBUG)
endfunction()

function(apply_debug_flags tgt)
    if(MSVC)
        target_compile_options(${tgt} PRIVATE /Od /Zi)
        if(NOT ENABLE_SANITIZERS)
            target_compile_options(${tgt} PRIVATE /RTC1)
        endif()
        target_compile_definitions(${tgt} PRIVATE _DEBUG)
    else()
        target_compile_options(${tgt} PRIVATE -O0 -g)
        if(ENABLE_PROFILING AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_compile_options(${tgt} PRIVATE -pg)
            target_link_options(${tgt}    PRIVATE -pg)
        endif()
    endif()
endfunction()

function(apply_valgrind_flags tgt)
    if(MSVC)
        target_compile_options(${tgt} PRIVATE /Od /Zi)
    else()
        target_compile_options(${tgt} PRIVATE -O0 -g)
    endif()
endfunction()

function(apply_benchmark_flags tgt)
    if(MSVC)
        target_compile_options(${tgt} PRIVATE /O2)
    else()
        target_compile_options(${tgt} PRIVATE -O3)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_compile_options(${tgt} PRIVATE -pg)
            target_link_options(${tgt}    PRIVATE -pg)
        endif()
    endif()
endfunction()

function(copy_resources tgt)
    if(EXISTS "${RES_DIR}")
        add_custom_command(TARGET ${tgt} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${RES_DIR}" "$<TARGET_FILE_DIR:${tgt}>/resources"
            COMMENT "Copying resources for ${tgt}"
            VERBATIM
        )
    endif()
endfunction()

if(MSVC)
    set(SPDLOG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog/include")
endif()

# ----------------------------------------------------------------------------
#  Main (release) executable
# ----------------------------------------------------------------------------
add_executable(main_app ${PROJECT_SOURCES})
set_target_properties(main_app PROPERTIES OUTPUT_NAME "main")
target_include_directories(main_app PRIVATE "${INC_DIR}")
# ORDER MATTERS: Warnings first, ReleaseFlags second.  See CompilerWarnings.cmake.
target_link_libraries(main_app PRIVATE
    Project::Warnings
    Project::ReleaseFlags
)
apply_release_flags(main_app)
copy_resources(main_app)

# ----------------------------------------------------------------------------
#  Dev (debug + sanitizers) executable
# ----------------------------------------------------------------------------
add_executable(dev_app ${PROJECT_SOURCES})
set_target_properties(dev_app PROPERTIES OUTPUT_NAME "dev")
target_include_directories(dev_app PRIVATE "${INC_DIR}")
target_link_libraries(dev_app PRIVATE Project::Warnings)
apply_debug_flags(dev_app)
if(ENABLE_SANITIZERS)
    target_link_libraries(dev_app PRIVATE Project::Sanitizers)
endif()
copy_resources(dev_app)

# ----------------------------------------------------------------------------
#  Valgrind executable
# ----------------------------------------------------------------------------
add_executable(valgrind_app ${PROJECT_SOURCES})
set_target_properties(valgrind_app PROPERTIES OUTPUT_NAME "valgrind")
target_include_directories(valgrind_app PRIVATE "${INC_DIR}")
target_link_libraries(valgrind_app PRIVATE Project::Warnings)
apply_valgrind_flags(valgrind_app)
copy_resources(valgrind_app)

# ----------------------------------------------------------------------------
#  Benchmarks
# ----------------------------------------------------------------------------
if(BUILD_BENCHMARKS)
    file(GLOB_RECURSE BENCH_SOURCES CONFIGURE_DEPENDS "${BENCH_DIR}/*.cpp")
    foreach(_f IN LISTS EXCLUDED_FILES)
        list(FILTER BENCH_SOURCES EXCLUDE REGEX "/${_f}$")
    endforeach()
    list(FILTER BENCH_SOURCES EXCLUDE REGEX "/main\\.cpp$")

    add_executable(benchmark_app ${BENCH_SOURCES} ${PROJECT_SUPPORT_SOURCES})
    set_target_properties(benchmark_app PROPERTIES OUTPUT_NAME "benchmarks")
    target_include_directories(benchmark_app PRIVATE "${INC_DIR}")
    target_link_libraries(benchmark_app PRIVATE Project::Warnings)

    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(benchmark_app PRIVATE benchmark::benchmark)

    find_package(Threads REQUIRED)
    target_link_libraries(benchmark_app PRIVATE Threads::Threads)

    apply_benchmark_flags(benchmark_app)
    copy_resources(benchmark_app)
endif()

# ----------------------------------------------------------------------------
#  Tests
#  NOTE: the executable's OUTPUT_NAME is "tests", not "test", because
#  enable_testing() already registers a phony "test" target and Ninja treats
#  both as file rules — leaving it as "test" triggers:
#      ninja: error: build.ninja:N: multiple rules generate test
# ----------------------------------------------------------------------------
if(BUILD_TESTS)
    set(_test_srcs)

    if(TEST_OPTION STREQUAL "full")
        file(GLOB_RECURSE _test_srcs CONFIGURE_DEPENDS "${TEST_DIR}/*.cpp")
        foreach(_f IN LISTS EXCLUDED_FILES)
            list(FILTER _test_srcs EXCLUDE REGEX "/${_f}$")
        endforeach()
    else()
        set(_folders)
        if(TEST_OPTION STREQUAL "integrations")
            list(APPEND _folders "${TEST_INTEG_DIR}")
        elseif(TEST_OPTION STREQUAL "mocks")
            list(APPEND _folders "${TEST_MOCKS_DIR}")
        endif()
        list(APPEND _folders "${TEST_UNIT_DIR}" "${TEST_MAIN_DIR}")

        foreach(_folder IN LISTS _folders)
            if(EXISTS "${_folder}")
                file(GLOB_RECURSE _tmp CONFIGURE_DEPENDS "${_folder}/*.cpp")
                list(APPEND _test_srcs ${_tmp})
            endif()
        endforeach()
    endif()

    list(APPEND _test_srcs "${TEST_MAIN_DIR}/testMain.cpp")
    list(REMOVE_DUPLICATES _test_srcs)
    list(APPEND _test_srcs ${PROJECT_SUPPORT_SOURCES})

    add_executable(test_app ${_test_srcs})
    set_target_properties(test_app PROPERTIES OUTPUT_NAME "tests")
    target_include_directories(test_app PRIVATE "${INC_DIR}")
    if(EXISTS "${TEST_INC_DIR}")
        target_include_directories(test_app PRIVATE "${TEST_INC_DIR}")
    endif()
    target_link_libraries(test_app PRIVATE Project::Warnings)

    if(TEST_STANDARD STREQUAL "catch2")
        find_package(Catch2 CONFIG REQUIRED)
        target_link_libraries(test_app PRIVATE Catch2::Catch2)
        target_compile_definitions(test_app PRIVATE catch2)
    elseif(TEST_STANDARD STREQUAL "googletest")
        find_package(GTest REQUIRED)
        target_link_libraries(test_app PRIVATE GTest::gtest)
        find_package(Threads REQUIRED)
        target_link_libraries(test_app PRIVATE Threads::Threads)
        target_compile_definitions(test_app PRIVATE googletest)
    else()
        message(FATAL_ERROR
            "Unsupported TEST_STANDARD='${TEST_STANDARD}'; expected catch2 or googletest")
    endif()

    if(ENABLE_COVERAGE AND NOT MSVC)
        target_link_libraries(test_app PRIVATE Project::Coverage)
    elseif(MSVC)
        target_compile_options(test_app PRIVATE /Od /Zi)
    else()
        target_compile_options(test_app PRIVATE -O0 -g)
    endif()

    copy_resources(test_app)

    enable_testing()

    if(TEST_STANDARD STREQUAL "googletest")
        include(GoogleTest)
        gtest_discover_tests(test_app)
    else()
        include(Catch)
        catch_discover_tests(test_app)
    endif()
endif()

# ----------------------------------------------------------------------------
#  Tracy profiling target
# ----------------------------------------------------------------------------
if(BUILD_TRACY)
    set(_tracy_srcs ${PROJECT_SOURCES})
    set(_tracy_client "${SRC_DIR}/tracy/public/TracyClient.cpp")
    if(EXISTS "${_tracy_client}")
        list(APPEND _tracy_srcs "${_tracy_client}")
    endif()

    add_executable(tracy_app ${_tracy_srcs})
    set_target_properties(tracy_app PROPERTIES OUTPUT_NAME "tracy-client")
    target_compile_definitions(tracy_app PRIVATE
        TRACY_ENABLE TRACY_TIMER_FALLBACK TRACY_ON_DEMAND)
    target_include_directories(tracy_app PRIVATE "${INC_DIR}")
    target_link_libraries(tracy_app PRIVATE
        Project::Warnings
        Project::ReleaseFlags
    )
    apply_release_flags(tracy_app)
    copy_resources(tracy_app)

    find_package(Threads REQUIRED)
    target_link_libraries(tracy_app PRIVATE Threads::Threads)
endif()

if(MSVC)
    target_compile_options(main_app     PRIVATE /std:c++latest)
    target_compile_options(dev_app      PRIVATE /std:c++latest)
    target_compile_options(valgrind_app PRIVATE /std:c++latest)
    target_compile_options(benchmark_app PRIVATE /std:c++latest)
    target_compile_options(test_app PRIVATE /std:c++latest)
endif()

if(MSVC)
    # Header-only mode: no library to link, just the include path.
    target_include_directories(main_app     SYSTEM PRIVATE "${SPDLOG_INCLUDE_DIR}")
    target_include_directories(dev_app      SYSTEM PRIVATE "${SPDLOG_INCLUDE_DIR}")
    target_include_directories(valgrind_app SYSTEM PRIVATE "${SPDLOG_INCLUDE_DIR}")
    target_include_directories(benchmark_app SYSTEM PRIVATE "${SPDLOG_INCLUDE_DIR}")
    target_include_directories(test_app SYSTEM PRIVATE "${SPDLOG_INCLUDE_DIR}")
endif()

# ----------------------------------------------------------------------------
#  Shader compilation (optional)
# ----------------------------------------------------------------------------
file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS "${RES_DIR}/shaders/*.slang")
find_program(SLANGC_EXECUTABLE slangc)

if(SLANGC_EXECUTABLE AND SHADER_SOURCES)
    set(_spv_outputs)
    foreach(_shader IN LISTS SHADER_SOURCES)
        get_filename_component(_name ${_shader} NAME_WE)
        set(_spv "${RES_DIR}/shaders/${_name}.spv")
        list(APPEND _spv_outputs ${_spv})

        add_custom_command(
            OUTPUT ${_spv}
            COMMAND ${SLANGC_EXECUTABLE} "${_shader}"
                    -target spirv -profile spirv_1_4
                    -emit-spirv-directly -fvk-use-entrypoint-name
                    -entry vertMain -entry fragMain
                    -o "${_spv}"
            DEPENDS "${_shader}"
            COMMENT "Compiling shader: ${_name}.slang"
            VERBATIM
        )
    endforeach()

    add_custom_target(shaders ALL DEPENDS ${_spv_outputs})
    add_dependencies(main_app     shaders)
    add_dependencies(dev_app      shaders)
    add_dependencies(valgrind_app shaders)
else()
    add_custom_target(shaders)
endif()

# ----------------------------------------------------------------------------
#  Run targets
# ----------------------------------------------------------------------------
add_custom_target(run_release
    COMMAND $<TARGET_FILE:main_app>
    DEPENDS main_app
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:main_app>"
    USES_TERMINAL COMMENT "Running release executable")

add_custom_target(run_dev
    COMMAND $<TARGET_FILE:dev_app>
    DEPENDS dev_app
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:dev_app>"
    USES_TERMINAL COMMENT "Running dev executable")

find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE AND NOT WIN32)
    add_custom_target(run_valgrind
        COMMAND ${VALGRIND_EXECUTABLE}
            --leak-check=full --show-leak-kinds=all -s
            --track-origins=yes --gen-suppressions=all --error-exitcode=1
            $<TARGET_FILE:valgrind_app>
        DEPENDS valgrind_app
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:valgrind_app>"
        USES_TERMINAL COMMENT "Running under Valgrind")
endif()

if(BUILD_BENCHMARKS)
    add_custom_target(run_benchmarks
        COMMAND $<TARGET_FILE:benchmark_app>
        DEPENDS benchmark_app
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:benchmark_app>"
        USES_TERMINAL COMMENT "Running benchmarks")
endif()

if(BUILD_TESTS)
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS test_app
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        USES_TERMINAL COMMENT "Running tests via CTest")
endif()

# ----------------------------------------------------------------------------
#  Coverage
# ----------------------------------------------------------------------------
if(ENABLE_COVERAGE AND NOT MSVC)
    find_program(LCOV_EXECUTABLE    lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(LCOV_EXECUTABLE)
        add_custom_target(coverage
            # 1) Zero out any previous counters so stale .gcda can't pollute.
            COMMAND ${LCOV_EXECUTABLE}
                --zerocounters
                -d "${CMAKE_BINARY_DIR}/CMakeFiles/test_app.dir"

            # 2) RUN the tests.  This is the step that was missing.
            COMMAND ${CMAKE_CTEST_COMMAND}
                --output-on-failure
                --test-dir "${CMAKE_BINARY_DIR}"

            # 3) Capture — scope to test_app's object dir only.
            COMMAND ${LCOV_EXECUTABLE}
                --capture
                -d "${CMAKE_BINARY_DIR}/CMakeFiles/test_app.dir"
                --config-file "${CMAKE_SOURCE_DIR}/.lcovrc"
                --branch-coverage
                --ignore-errors gcov,gcov,inconsistent,inconsistent,mismatch,mismatch,unused,unused
                -o "${CMAKE_BINARY_DIR}/coverage.raw.info"

            # 4) Keep only project sources, anchored to the project root.
            COMMAND ${LCOV_EXECUTABLE}
                --extract "${CMAKE_BINARY_DIR}/coverage.raw.info"
                "${CMAKE_SOURCE_DIR}/src/*"
                "${CMAKE_SOURCE_DIR}/include/*"
                "${CMAKE_SOURCE_DIR}/testInclude/*"
                --config-file "${CMAKE_SOURCE_DIR}/.lcovrc"
                --branch-coverage
                --ignore-errors gcov,gcov,inconsistent,inconsistent,mismatch,mismatch,unused,unused
                -o "${CMAKE_BINARY_DIR}/coverage.tmp.info"

            COMMAND ${CMAKE_COMMAND} -E rename
                "${CMAKE_BINARY_DIR}/coverage.tmp.info"
                "${CMAKE_BINARY_DIR}/coverage.info"

            DEPENDS test_app
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Running tests and generating coverage.info"
            USES_TERMINAL
        )
    endif()

    if(GENHTML_EXECUTABLE)
        add_custom_target(genhtml
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/coverage"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/coverage"
            COMMAND ${GENHTML_EXECUTABLE}
                "${CMAKE_BINARY_DIR}/coverage.info"
                --config-file "${CMAKE_SOURCE_DIR}/.lcovrc"
                --branch-coverage
                --ignore-errors empty,unused
                --dark-mode
                -o "${CMAKE_BINARY_DIR}/coverage"
            DEPENDS coverage
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Generating HTML coverage report"
            USES_TERMINAL
        )
    endif()
endif()

# ----------------------------------------------------------------------------
#  Static analysis & formatting
# ----------------------------------------------------------------------------
find_program(CLANG_TIDY_EXECUTABLE     NAMES clang-tidy)
find_program(RUN_CLANG_TIDY_EXECUTABLE NAMES run-clang-tidy run-clang-tidy.py)
find_program(CPPCHECK_EXECUTABLE       NAMES cppcheck)
find_program(FLAWFINDER_EXECUTABLE     NAMES flawfinder)
find_program(CLANG_FORMAT_EXECUTABLE   NAMES clang-format)

# Source list for clang-tidy / cppcheck / flawfinder — mirrors CLANG_SOURCES
file(GLOB_RECURSE ANALYSIS_SOURCES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"
    "${TEST_DIR}/*.cpp"
)
foreach(_f IN LISTS EXCLUDED_FILES)
    list(FILTER ANALYSIS_SOURCES EXCLUDE REGEX "/${_f}$")
endforeach()
foreach(_d IN LISTS EXCLUDED_FOLDERS)
    list(FILTER ANALYSIS_SOURCES EXCLUDE REGEX "/${_d}/")
endforeach()

# Source list for clang-format — mirrors CLANG_SOURCES + INCLUDE_SOURCES
file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"   "${SRC_DIR}/*.h"   "${SRC_DIR}/*.hpp"
    "${TEST_DIR}/*.cpp"  "${TEST_DIR}/*.h"  "${TEST_DIR}/*.hpp"
    "${INC_DIR}/*.h"     "${INC_DIR}/*.hpp"
)
foreach(_f IN LISTS EXCLUDED_FILES)
    list(FILTER FORMAT_SOURCES EXCLUDE REGEX "/${_f}$")
endforeach()
foreach(_d IN LISTS EXCLUDED_FOLDERS)
    list(FILTER FORMAT_SOURCES EXCLUDE REGEX "/${_d}/")
endforeach()

# --- clang-tidy (single-threaded, one file at a time) ----------------------
if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXECUTABLE}
            -p ${CMAKE_BINARY_DIR}
            --config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
            --extra-arg=-Wno-error
            ${ANALYSIS_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-tidy"
        USES_TERMINAL VERBATIM
    )
endif()

# --- run-clang-tidy (parallel) ---------------------------------------------
if(RUN_CLANG_TIDY_EXECUTABLE)
    add_custom_target(run_tidy
        COMMAND ${RUN_CLANG_TIDY_EXECUTABLE}
            -p ${CMAKE_BINARY_DIR}
            -config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
            -j
            -use-color=true
            -extra-arg=-Wno-error
            -extra-arg=-Wno-unknown-warning-option
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running run-clang-tidy (parallel)"
        USES_TERMINAL VERBATIM
    )
endif()

# --- cppcheck --------------------------------------------------------------
if(CPPCHECK_EXECUTABLE)
    add_custom_target(check
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/cppcheck"
        COMMAND ${CPPCHECK_EXECUTABLE}
            "--project=${CMAKE_BINARY_DIR}/compile_commands.json"
            "--cppcheck-build-dir=${CMAKE_BINARY_DIR}/cppcheck"
            --check-level=exhaustive
            --inline-suppr
            --suppress=missingIncludeSystem
            --enable=warning,style,performance,portability
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running cppcheck"
        USES_TERMINAL
        VERBATIM
    )
endif()

# --- flawfinder ------------------------------------------------------------
if(FLAWFINDER_EXECUTABLE)
    add_custom_target(flawfinder
        COMMAND ${FLAWFINDER_EXECUTABLE}
            --minlevel=1
            --falsepositive
            --columns
            ${ANALYSIS_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running flawfinder"
        USES_TERMINAL
        VERBATIM
    )
endif()

# --- clang-format (in-place) -----------------------------------------------
if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE}
            -i
            -style=file:"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
            ${FORMAT_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Formatting sources with clang-format"
        USES_TERMINAL
        VERBATIM
    )

    # Dry-run variant suitable for CI.
    add_custom_target(format_check
        COMMAND ${CLANG_FORMAT_EXECUTABLE}
            --dry-run --Werror
            -style=file:"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
            ${FORMAT_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Checking clang-format compliance"
        USES_TERMINAL
        VERBATIM
    )
endif()

# --- `analysis` aggregator (equivalent to `make analysis`) -----------------
add_custom_target(analysis)
if(TARGET flawfinder)
    add_dependencies(analysis flawfinder)
endif()
if(TARGET check)
    add_dependencies(analysis check)
endif()
if(TARGET tidy)
    add_dependencies(analysis tidy)
endif()
