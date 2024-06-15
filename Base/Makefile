COMPILER = g++
COMPILER_VERSION = -std=c++2a
COMPILER_FLAGS_RELEASE = ${COMPILER_VERSION} -O3 -DNDEBUG
COMPILER_FLAGS_DEV = ${COMPILER_VERSION} -O0 -g -pg

WARNINGS = -pedantic -pedantic-errors -Wall -Wextra -Weffc++ -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Wno-unused -Wfloat-equal -Wconversion -Winline -Wzero-as-null-pointer-constant -Wmissing-noreturn -Wunreachable-code -Wvariadic-macros -Wwrite-strings -Wunsafe-loop-optimizations -Werror

INCLUDE_FOLDER = include
INCLUDE_ARGUMENT = -I${INCLUDE_FOLDER}
LIBRARIES =
RESOURCES_FOLDER = .\/resources

SOURCE_FOLDER = src
SOURCES = $(shell find ${SOURCE_FOLDER} -name '*.cpp')
INCLUDE_SOURCES = $(shell find ${INCLUDE_FOLDER} -type f \( -name '*.h' -o -name '*.hpp' \))

BUILD_FOLDER = build
RELEASE_FOLDER = release
DEV_FOLDER = dev
OUTPUT_FOLDER_RELEASE = ${BUILD_FOLDER}/${RELEASE_FOLDER}
OUTPUT_FOLDER_DEV = ${BUILD_FOLDER}/${DEV_FOLDER}
OUTPUT_FILE_RELEASE = main
OUTPUT_FILE_DEV = dev

VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --verbose -s --track-origins=yes

TEST_FOLDER = tests
TEST_SOURCES = $(shell find ${TEST_FOLDER} ${SOUCE_FOLDER} -type f -not -path '*/main.cpp' -name '*.cpp')
TEST_COMPILER_FLAGS = ${COMPILER_VERSION} --coverage -fPIC -O0
TEST_INCLUDE_FOLDER =
TEST_INCLUDE_ARGUMENT =
TEST_LIBRARIES = ${LIBRARIES} -lgcov -lgtest -lpthread
TEST_RESOURCES =
TEST_OUTPUT_FOLDER = ${BUILD_FOLDER}/${TEST_FOLDER}
TEST_OUTPUT_FILE = test

BRANCH_COVERAGE = --rc lcov_branch_coverage=1

LCOV_FILES = $(shell find . -name '*.gcno' -o -name '*.gcda' -o -name '*.info')
LCOV_FLAGS = --no-external --capture ${BRANCH_COVERAGE} -d .
LCOV_OUTPUT_FOLDER = ${TEST_OUTPUT_FOLDER}
LCOV_OUTPUT_FILE = coverage.info
LCOV_REMOVE_FILES = '*/${TEST_FOLDER}/*'

GENHTML_FLAGS = ${BRANCH_COVERAGE}
GENHTML_OUTPUT_FOLDER = .\/coverage

CLANG_SOURCES = ${SOURCES}
TIDY_COMPILE_FLAGS = -extra-arg=-std=c++20 --config-file=.clang-tidy
FORMAT_COMPILE_FLAGS = -i -style=file:.clang-format

CPPCHECK_FOLDER = cppcheck
CPPCHECK_COMPILE_FLAGS = --cppcheck-build-dir=${CPPCHECK_FOLDER} --check-level=exhaustive

PROFILE_FOLDER = .\/profiling
PROFILE_ANNOTATIONS_FOLDER = ${PROFILE_FOLDER}/annotations

ANNOTATION_FILES = $(shell find . -maxdepth 1 -type f -name '*-ann')

GPROF_COMPILER_FLAGS = -A -b -p -y
GPROF_FILE = flat.map

OBJECTS_RELEASE = $(SOURCES:${SOURCE_FOLDER}/%.cpp=${OUTPUT_FOLDER_RELEASE}/%.o)
DEPS_RELEASE = $(OBJECTS_RELEASE:.o=.d)
OBJECTS_DEV = $(SOURCES:${SOURCE_FOLDER}/%.cpp=${OUTPUT_FOLDER_DEV}/%.o)
DEPS_DEV = $(OBJECTS_DEV:.o=.d)
OBJECTS_TEST = $(TEST_SOURCES:${TEST_FOLDER}/%.cpp=${TEST_OUTPUT_FOLDER}/%.o)
DEPS_TEST = $(OBJECTS_TEST:.o=.d)

default: compile

create_folders:
	mkdir -p ${RESOURCES_FOLDER}
	mkdir -p ${OUTPUT_FOLDER_RELEASE}
	mkdir -p ${OUTPUT_FOLDER_DEV}
	mkdir -p ${TEST_OUTPUT_FOLDER}

${OUTPUT_FOLDER_RELEASE}/%.o: ${SOURCE_FOLDER}/%.cpp
	${COMPILER} ${COMPILER_FLAGS_RELEASE} ${WARNINGS} ${INCLUDE_ARGUMENT} -MMD -MP -c $< -o $@

-include $(DEPS_RELEASE)

compile: create_folders $(OBJECTS_RELEASE)
	${COMPILER} ${COMPILER_FLAGS_RELEASE} ${OBJECTS_RELEASE} ${LIBRARIES} -o ${OUTPUT_FOLDER_RELEASE}/${OUTPUT_FILE_RELEASE}
	cp -r ${RESOURCES_FOLDER} ${OUTPUT_FOLDER_RELEASE}

release: compile
	${OUTPUT_FOLDER_RELEASE}/${OUTPUT_FILE_RELEASE}

${OUTPUT_FOLDER_DEV}/%.o: ${SOURCE_FOLDER}/%.cpp
	${COMPILER} ${COMPILER_FLAGS_DEV} ${WARNINGS} ${INCLUDE_ARGUMENT} -MMD -MP -c $< -o $@

-include $(DEPS_DEV)

debug: create_folders $(OBJECTS_DEV)
	${COMPILER} ${COMPILER_FLAGS_DEV} ${OBJECTS_DEV} ${LIBRARIES} -o ${OUTPUT_FOLDER_DEV}/${OUTPUT_FILE_DEV}
	cp -r ${RESOURCES_FOLDER} ${OUTPUT_FOLDER_DEV}

dev: debug
	${OUTPUT_FOLDER_DEV}/${OUTPUT_FILE_DEV}
	rm -rf gmon.out

valgrind: debug
	valgrind ${VALGRIND_FLAGS} ${OUTPUT_FOLDER_DEV}/${OUTPUT_FILE_DEV}

${TEST_OUTPUT_FOLDER}/%.o: ${TEST_FOLDER}/%.cpp
	${COMPILER} ${TEST_COMPILER_FLAGS} ${WARNINGS} ${INCLUDE_ARGUMENT} ${TEST_INCLUDE_ARGUMENT} -MMD -MP -c $< -o $@

copy_and_run_test:
	cp -r ${RESOURCES_FOLDER} ${OUTPUT_FOLDER_DEV}
	cp ${TEST_OUTPUT_FOLDER}/${TEST_OUTPUT_FILE} ${OUTPUT_FOLDER_DEV}/${TEST_OUTPUT_FILE}
	${OUTPUT_FOLDER_DEV}/${TEST_OUTPUT_FILE}

-include $(DEPS_TEST)

build_test: create_folders $(OBJECTS_TEST)
	${COMPILER} ${TEST_FLAGS_RELEASE} ${OBJECTS_TEST} ${TEST_LIBRARIES} -o ${TEST_OUTPUT_FOLDER}/${TEST_OUTPUT_FILE}
	${MAKE} copy_and_run_test

lcov: build_test
	mkdir -p ${LCOV_OUTPUT_FOLDER}
	lcov ${LCOV_FLAGS} -o ${LCOV_OUTPUT_FOLDER}/${LCOV_OUTPUT_FILE}
	lcov --remove ${LCOV_OUTPUT_FOLDER}/${LCOV_OUTPUT_FILE} ${LCOV_REMOVE_FILES} ${BRANCH_COVERAGE} -o ${LCOV_OUTPUT_FOLDER}/${LCOV_OUTPUT_FILE}

genhtml: lcov
	rm -rf ${GENHTML_OUTPUT_FOLDER}
	mkdir -p ${GENHTML_OUTPUT_FOLDER}
	genhtml ${LCOV_OUTPUT_FOLDER}/${LCOV_OUTPUT_FILE} ${BRANCH_COVERAGE} --output-directory ${GENHTML_OUTPUT_FOLDER}

coverage: genhtml
	cp ${TEST_OUTPUT_FOLDER}/coverage.info ${GENHTML_OUTPUT_FOLDER}/lcov.info
	rm -rf ${TEST_OUTPUT_FOLDER}
	rm -rf ${LCOV_OUTPUT_FOLDER}

tidy: ${CLANG_SOURCES}
	clang-tidy ${TIDY_COMPILE_FLAGS} $^ -- ${INCLUDE_ARGUMENT}

cppcheck: ${CLANG_SOURCES}
	mkdir -p ${CPPCHECK_FOLDER}
	cppcheck ${CPPCHECK_COMPILE_FLAGS} $^ -I ${INCLUDE_FOLDER}/

flawfinder: ${CLANG_SOURCES}
	flawfinder $^

analysis: tidy cppcheck flawfinder

format: ${CLANG_SOURCES} ${INCLUDE_SOURCES}
	clang-format ${FORMAT_COMPILE_FLAGS} $^

run_doxygen:
	doxygen Doxyfile
	rm -f Doxyfile.bak

docs: run_doxygen
	sphinx-autobuild -b html -Dbreathe_projects.documentation=docs/xml . docs/sphinx/

gprof: dev
	mkdir -p ${PROFILE_FOLDER}
	mkdir -p ${PROFILE_ANNOTATIONS_FOLDER}
	rm -rf ${ANNOTATION_FILES}
	mv gmon.out ${PROFILE_FOLDER}/gmon.out
	gprof ${GPROF_COMPILER_FLAGS} ${OUTPUT_FOLDER_DEV}/${OUTPUT_FILE_DEV} ${PROFILE_FOLDER}/gmon.out > ${PROFILE_FOLDER}/${GPROF_FILE}

profile: gprof
	mv ${ANNOTATION_FILES} ${PROFILE_ANNOTATIONS_FOLDER}

initialize_repo:
	git clone https://github.com/Phaysik/CPPBase
	cp -ra CPPBase/Base/. .
	rm -rf CPPBase

.PHONY: tidy run_doxygen create_folders initialize_repo copy_and_run_test