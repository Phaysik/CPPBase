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
        sudo apt-get install -y make cmake valgrind graphviz libgtest-dev lcov python3-pip flex bison

        echo "Setting up g++"
        sudo apt install -y software-properties-common
        sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
        sudo apt update
        sudo apt-get install -y g++-13
        sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 13
        sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-13 13

        echo "Setting up Doxygen"
        wget https://doxygen.nl/files/doxygen-1.9.8.src.tar.gz
        tar -xzvf doxygen-1.9.8.src.tar.gz
        cd doxygen-1.9.8
        mkdir build
        cd build
        cmake -G "Unix Makefiles" ..
        make
        sudo make install
        cd ..
        cd ..
        rm -rf doxygen-1.9.8*

        echo "Setting up clang-tidy and clang-format"
        wget https://apt.llvm.org/llvm.sh
        chmod +x llvm.sh
        sudo ./llvm.sh 18
        rm -rf ./llvm.sh
        sudo apt-get install -y clang-format clang-tidy

        echo "Installing Google Test to the local machine for running the test suite"
        cd /usr/src/gtest
        sudo cmake CMakeLists.txt
        sudo make
        sudo cp ./lib/libgtest*.a /usr/lib

        echo "Installing Sphinx and it's dependencies for documentation"
        pip3 install --upgrade pip
        pip3 install sphinx breathe sphinx-book-theme sphinx-copybutton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page
    else
        echo -e "\nBegin by installing make itself, and then look at the table below to find what other packages to install based on what commands you wish to run\n"

        col1_width=29
        col2_width=49
        col3_width=30

        # Print the table header
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "Makefile Command" "Makefile Command(s) it relies on" "Packages Required"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "----------------" "--------------------------------" "-----------------"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "compile" "-" "make g++"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "run" "compile" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "debug" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "valgrind" "debug" "make valgrind"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "delete_lcov" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "copy_and_run_test" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "build_test" "create_folders, delete_lcov, copy_and_run_test" "make g++ libgtest-dev (May require extra installation steps - Look at guide online)"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "lcov" "build_test" "make lcov"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "genhtml" "lcov" "make lcov"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "coverage" "genhtml" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "tidy" "-" "make clang-tidy"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "format" "-" "make clang-format"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "run_doxygen" "-" "make graphviz doxygen flex bison"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "docs" "run_doxygen" "make sphinx breathe sphinx-book-theme sphinx-copybtton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_folders" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "initialize_repo" "-" "make git"
    fi
}

main "$@"