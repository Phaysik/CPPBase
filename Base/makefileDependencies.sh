#!/bin/bash

setUpDoxygen() {
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
}

setUpClangTools() {
    echo "Setting up clang-tidy and clang-format"
    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    sudo ./llvm.sh 19
    rm -rf ./llvm.sh
    sudo apt-get install -y clang-format clang-tidy
}

setUpCppCheck() {
    git clone https://github.com/danmar/cppcheck.git
    cd cppcheck
    mkdir build
    cd build
    cmake -DHAVE_RULES=ON -DBUILD_TESTS=ON ..
    cmake --build .
    cd bin
    sudo cp -r ./* /usr/bin/
    cd ..
    cd ..
    cd ..
    rm -rf cppcheck
}

checkSphinx() {
    packages=("sphinx" "breathe" "sphinx-book-theme" "sphinx-copybutton" "sphinx-autobuild" "sphinx-last-updated-by-git" "sphinx-notfound-page")

    all_packages_installed=true

    for package in "${packages[@]}"; do
        if ! pip show "$package" >/dev/null 2>&1; then
            echo "$package is not installed"
            all_packages_installed=false
        fi
    done

    if $all_packages_installed; then
        return 0  # All packages installed
    else
        return 1  # Some packages missing
    fi
}

main() {
    echo "This shell file is set up to only work on Ubuntu operating systems"

    response="n"

    if [ -z "$1" ]; then
        read -p "Enter (Y/N) if you are running on Ubuntu and wish to auto install all packages required: " response
    fi

    # a or 'A' for automated running (For Github workflows ignoring long documentation, linting, and formatting installation)
    if [ "${response,,}" = "y" ] || [ "${1,,}" = "y" ] || [ "${response,,}" = "a" ] || [ "${1,,}" = "a" ]; then
        echo "Update and upgrading your packages (will require an elevated user's password)"
        sudo apt update && sudo apt upgrade -y

        echo "Installing all the required packages for all commands used in the Makefile"

        sudo apt-get install -y make cmake valgrind graphviz libgtest-dev lcov python3-pip flex bison libpcre3 libpcre3-dev

        if [ -x "$(command -v g++-13)" ]; then
            echo "g++-13 exists"
        else
            echo "Setting up g++"
            sudo apt install -y software-properties-common
            sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
            sudo apt update
            sudo apt-get install -y g++-13
            sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 13
            sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-13 13
        fi

        if [ -f "/usr/lib/libgtest.a" ] && [ -f "/usr/lib/libgtest_main.a" ]; then
            echo "Google Test already exists"
        else
            echo "Setting up Google Test"
            cd /usr/src/gtest
            sudo cmake CMakeLists.txt
            sudo make
            sudo cp ./lib/libgtest*.a /usr/lib
        fi

        # If not 'a' or 'A', set up documentation, formatting, and linting tools
        if [ "${response,,}" = "y" ] || [ "${1,,}" = "y" ]; then
            if [ -x "$(command -v doxygen)" ]; then
                echo "Doxygen already exists"

                version=$(doxygen --version | awk '{print $1}')
                desired_version="1.9.8"

                if [[ "$version" == "$desired_version" ]]; then
                    echo "Doxygen version $desired_version already exists"
                else
                    setUpDoxygen
                fi
            else
                setUpDoxygen
            fi

            if [ -x "$(command -v cppcheck)" ]; then
                echo "Cppcheck already exists"

                version=$(cppcheck --version | awk '{print $1}')
                desired_version="2.15 dev"

                if [[ "$version" == "$desired_version" ]]; then
                    echo "Cppcheck version $desired_version already exists"
                else
                    setUpCppCheck
                fi
            else
                setUpCppCheck
            fi


            if [ -x "$(command -v clang-tidy)" ] && [ -x "$(command -v clang-format)" ]; then
                echo "clang-tidy and clang-format already exists"

                clang_tidy_version=$(clang-tidy --version | awk '/LLVM version/ {print $4}')
                clang_tidy_desired_version="19.0.0"

                clang_format_version=$(clang-format --version | awk '{print $4}')
                clang_format_desired_version="19.0.0"

                if [[ "$clang_tidy_version" == "$clang_tidy_desired_version" ]] && [[ "$clang_format_version" == "$clang_format_desired_version" ]]; then
                    echo "clang-tidy version $clang_tidy_desired_version and clang-format version $clang_format_desired_version already exists"
                else
                    setUpClangTools
                fi
            else
                setUpClangTools
            fi

            checkSphinx
            status=$?

            if [ $status -eq 0 ]; then
                echo "All Sphinx packages are installed"
            else
                echo "Installing Sphinx and it's dependencies for documentation"
                pip3 install --upgrade pip
                pip3 install sphinx breathe sphinx-book-theme sphinx-copybutton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page
            fi
        fi
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
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "cppcheck" "-" "make cppcheck"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "analysis" "tidy cppcheck" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "format" "-" "make clang-format"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "run_doxygen" "-" "make graphviz doxygen flex bison"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "docs" "run_doxygen" "make sphinx breathe sphinx-book-theme sphinx-copybtton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_folders" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "initialize_repo" "-" "make git"
    fi
}

main "$@"