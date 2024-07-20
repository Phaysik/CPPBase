#!/bin/bash

setUpGCC() {
    curl -L -o gcc-latest.tar.gz https://ftp.gnu.org/gnu/gcc/gcc-$1/gcc-$1.tar.gz
    mkdir -p gcc-latest
    tar -xvzf gcc-latest.tar.gz -C gcc-latest --strip-components=1

    sudo rm -rf gcc-latest
    cd gcc-latest
    ./contrib/download_prerequisites
    cd ..

    mkdir gcc-build
    cd gcc-build
    directory = /usr/local/gcc-14
    sudo mkdir $directory
    ../gcc-latest/configure --prefix=$directory --enable-languages=c,c++ --disable-multilib
    sudo make -j$(nproc)
    sudo make install

    sudo update-alternatives --install /usr/bin/g++ g++ $directory/bin/g++ 14
    sudo update-alternatives --install /usr/bin/gcc gcc $directory/bin/gcc 14
    sudo update-alternatives --install /usr/bin/gcov gcov $directory/bin/gcov 14

    sudo rm -rf gcc-latest
    sudo rm -rf gcc-build

    sudo mv /lib/x86_64-linux-gnu/libstdc++.so.6 /lib/x86_64-linux-gnu/libstdc++.so-copy.6
    sudo cp /usr/local/gcc-14/lib64/libstdc++.so.6 /lib/x86_64-linux-gnu/
}

setUpClangTools() {
    echo "Setting up clang-tidy and clang-format"
    wget https://apt.llvm.org/llvm.sh
    sudo chmod +x llvm.sh
    sudo ./llvm.sh 19
    rm -rf ./llvm.sh
    sudo apt-get install -y clang-format clang-tidy
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
    sudo rm -rf tracy-latest.tar.gz

    cd tracy-latest

    sudo apt-get install g++-11 gcc-11 libfreetype6-dev libcapstone-dev libegl1-mesa-dev libxkbcommon-dev libwayland-dev libdbus-1-dev libglfw3 libglfw3-dev -y
    sudo cp -r /usr/include/freetype2/* /usr/include/
    sudo cp -r /usr/include/capstone/* /usr/include/
    sudo cp -r /usr/include/dbus-1.0/* /usr/include/
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 11
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 11

    # For installing the Client of Tracy
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    cd ..

    # For installing the Server of Tracy
    cd profiler/build/unix
    make CXX=g++-11 LEGACY=1
    mv Tracy-release Tracy-Server
    sudo cp Tracy-Server /usr/bin/

    # Remove Tracy files from local
    cd ../../../../
    sudo rm -rf tracy-latest

    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14

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

        sudo apt-get install make cmake libgtest-dev python3-pip docker-compose -y

        sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 10
        sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-14 14
        sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 10

        desired_version="14.1.0"
        echo "Setting up g++"

        if [ "$(command g++ --version | grep -oP '\d+\.\d+\.\d+')" = "$desired_version" ]; then
            echo "g++-${desired_version} exists"
        elif [ "${1,,}" != "a" ] || ([ "${response,,}" != "a" ] && [ "${response,,}" != "n" ]); then
            setUpGCC desired_version
        else
            sudo apt-get install -y g++-13 # For automated running
            sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 13
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
            sudo apt-get install binutils valgrind graphviz flex bison libpcre3 libpcre3-dev lcov cppcheck doxygen xterm -y

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
                pip3 install sphinx breathe sphinx-book-theme sphinx-copybutton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page --break-system-packages
            fi

            if [ -x "$(command -v flawfinder)" ]; then
                echo "Flawfinder already exists"
            else
                pip3 install flawfinder --break-system-packages
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
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "check" "-" "make libpcre3 libpcre3-dev cppcheck"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "flawfinder" "-" "make pip"
        printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "analysis" "tidy check flawfinder" "make"
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