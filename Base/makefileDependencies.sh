#!/bin/bash

setUpGCC() {
    echo "Setting up g++"
    curl -L -o gcc-latest.tar.gz https://ftp.gnu.org/gnu/gcc/gcc-$1/gcc-$1.tar.gz
    mkdir -p gcc-latest
    tar -xvzf gcc-latest.tar.gz -C gcc-latest --strip-components=1

    cd gcc-latest
    ./contrib/download_prerequisites
    cd ..

    mkdir gcc-build
    cd gcc-build
    ../gcc-latest/configure --prefix=/usr/local/gcc-14 --enable-languages=c,c++ --disable-multilib
    sudo make -j$(nproc)
    sudo make install

    sudo rm -rf /usr/bin/g++
    sudo rm -rf /usr/bin/gcc
    sudo rm -rf /usr/bin/gcov
    sudo ln -s /usr/local/gcc-14/bin/g++ /usr/bin/g++
    sudo ln -s /usr/local/gcc-14/bin/gcc /usr/bin/gcc
    sudo ln -s /usr/local/gcc-14/bin/gcov /usr/bin/gcov
}

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

setUpTracy() {
    # Download the latest version of Tracy
    url=$(curl -s https://api.github.com/repos/wolfpld/tracy/releases/latest | grep -o '"tarball_url": *"[^"]*"' | cut -d '"' -f 4)
    curl -L -o tracy-latest.tar.gz $url
    mkdir -p tracy-latest
    tar -xvzf tracy-latest.tar.gz -C tracy-latest --strip-components=1
    cd tracy-latest

    # For installing the Client of Tracy
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    cd ..

    # For installing the Server of Tracy
    cd profiler/build/unix
    make CXX=g++-11
    mv Tracy-release Tracy-Server
    sudo cp Tracy-Server /usr/bin/

    # Remove Tracy files from local
    cd ../../../../
    sudo rm -rf tracy
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

        sudo apt-get install -y build-essential libgmp3-dev libmpc-dev libmpfr-dev texinfo make cmake valgrind graphviz libgtest-dev lcov python3-pip flex bison libpcre3 libpcre3-dev binutils

        desired_version="14.1.0"

        if [ "$(command g++ --version | grep -oP '\d+\.\d+\.\d+')" = "$desired_version" ]; then
            echo "g++-${desired_version} exists"
        else
            setUpGCC desired_version
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

                version=$(cppcheck --version | awk '{print $2}')
                desired_version="2.15"

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

            if [ -x "$(command -v flawfinder)" ]; then
                echo "Flawfinder already exists"
            else
                pip3 install flawfinder
            fi

            if [ -x "$(command -v tracy-Server)" ]; then
                echo "Tracy-Server already exists"
            else
                setUpTracy
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
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "create_folders" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "compile" "create_folders" "make g++"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "release" "compile" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "debug" "create_folders" "make g++"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "dev" "debug" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "val" "create_folders" "make g++"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "valgrind" "val" "make valgrind"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "copy_and_run_test" "-" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "build_test" "create_folders, copy_and_run_test" "make g++ libgtest-dev (May require extra installation steps - Look at guide online)"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "lcov" "build_test" "make lcov"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "genhtml" "lcov" "make lcov"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "coverage" "genhtml" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "tidy" "-" "make clang-tidy"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "cppcheck" "-" "make libpcre3 libpcre3-dev cppcheck"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "flawfinder" "-" "make pip"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "analysis" "tidy cppcheck flawfinder" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "format" "-" "make clang-format"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "run_doxygen" "-" "make graphviz doxygen flex bison"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "profile" "dev" "make binutils"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "docs" "run_doxygen" "make sphinx breathe sphinx-book-theme sphinx-copybtton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "tracy" "create_folders" "make g++-11"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "gprof" "dev" "make binutils"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "profile" "gprof" "make"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "initialize_repo" "-" "make git"
    fi
}

main "$@"