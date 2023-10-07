#!/bin/bash

main() {
    echo "This shell file is set up to only work on Ubuntu operating systems"

    response="n"

    if [ -z "$1" ]; then
        read -p "Enter (Y/N) if you are running on Ubuntu and wish to auto install all packages required: " response
    fi

    if [ "${response,,}" = "y" ] || [ "${1,,}" = "y" ]; then
        echo "Update and upgrading your packages (will require an elevated user's password)"

        sudo apt update && sudo apt upgrade -y

        echo "Installing all the required packages for all commands used in the Makefile"
        sudo apt-get install -y make cmake valgrind graphviz doxygen clang-tidy libgtest-dev lcov

        echo "Installing Google Test to the local machine for running the test suite"
        cd /usr/src/gtest
        sudo cmake CMakeLists.txt
        sudo make
        sudo cp ./lib/libgtest*.a /usr/lib

        echo "Installing Sphinx and it's dependencies for documentation"
        pip3 install sphinx breathe sphinx-book-theme sphinx-copybutton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page
    else
        echo -e "\nBegin by installing make itself, and then look at the table below to find what other packages to install based on what commands you wish to run\n"

        col1_width=29
        col2_width=49
        col3_width=30

        # Print the table header
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "Makefile Command" "Makefile Command(s) it relies on" "Packages Required"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "----------------" "--------------------------------" "-----------------"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "compile" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "run" "compile" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "debug" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "valgrind" "debug" "make valgrind"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "delete_lcov" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "copy_and_run_test" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "build_test" "create_folders, delete_lcov, copy_and_run_test" "make libgtest-dev (May require extra installation steps - Look at guide online)"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "lcov" "build_test" "make lcov"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "genhtml" "lcov" "make lcov"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "coverage" "genhtml" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "tidy_setup" "-" "make clang-tidy"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "fix_tidy_warnings" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "tidy" "tidy_setup, fix_tidy_warnings" "make clang-tidy"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_doxygen" "-" "make doxygen"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "change_doxygen_options" "create_doxygen" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_breathe_file" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_index_rst_file" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "determine_index_rst_exists" "create_breathe_file, create_index_rst_file" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "doxygen_complete" "change_doxygen_options" "make graphviz doxygen"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "docs" "doxygen_complete, determine_index_rst_exists" "make sphinx breathe sphinx-book-theme sphinx-copybtton sphinx-autobuild sphinx-last-updated-by-git"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_folders" "-" "make"
    fi
}

main "$@"