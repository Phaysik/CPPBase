#!/bin/bash

setUpGCC() {
	echo "Setting up gcc, g++, and gcov"

	curl -L -o gcc-latest.tar.gz https://ftp.gnu.org/gnu/gcc/gcc-"$1"/gcc-"$1".tar.gz
	mkdir -p gcc-latest
	tar -xvzf gcc-latest.tar.gz -C gcc-latest --strip-components=1

	sudo rm -rf gcc-latest.tar.gz
	cd gcc-latest || exit
	./contrib/download_prerequisites
	cd ..

	directory="/usr/local/gcc-$2"
	sudo mkdir -p "${directory}"

	# clean any previous incomplete build
	if [[ -d gcc-build ]]; then
		rm -rf gcc-build
	fi

	mkdir -p gcc-build
	cd gcc-build || exit

	../gcc-latest/configure --prefix="${directory}" --enable-languages=c,c++ --disable-multilib --enable-bootstrap
	make -j"$(nproc)" || true
	sudo make install

	sudo ldconfig

	sudo update-alternatives --install /usr/bin/g++ g++ "${directory}"/bin/g++ "$2"
	sudo update-alternatives --install /usr/bin/gcc gcc "${directory}"/bin/gcc "$2"
	sudo update-alternatives --install /usr/bin/gcov gcov "${directory}"/bin/gcov "$2"
	sudo update-alternatives --install /usr/bin/c++ c++ "${directory}"/bin/c++ "$2"

	cd ..

	sudo rm -rf gcc-latest
	sudo rm -rf gcc-build

	sudo mv /lib/x86_64-linux-gnu/libstdc++.so.6 /lib/x86_64-linux-gnu/libstdc++.so-copy.6

	sudo ln -sf /usr/local/gcc-"$2"/lib64/libstdc++.so.6 /lib/x86_64-linux-gnu
	sudo ln -sf /usr/local/gcc-"$2"/lib64/libstdc++.so.6.0.36 /lib/x86_64-linux-gnu
	sudo ldconfig
}

setUpClang() {
    echo "Setting up clang++, clang-tidy, clang-format, and clangd"

	wget https://apt.llvm.org/llvm.sh
	sudo chmod +x llvm.sh
	sudo ./llvm.sh "$1"
	rm -rf ./llvm.sh
	sudo apt-get update
	sudo apt-get install -y clang-format-"$1" clang-tidy-"$1" clangd-"$1" clang++-"$1"

	sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-"$1" "$1"
	sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-"$1" "$1"
	sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-"$1" "$1"
	sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-"$1" "$1"
}

installDoxygen() {
	echo "Setting up doxygen"

	sudo wget https://www.doxygen.nl/files/doxygen-"$1".linux.bin.tar.gz

	mkdir -p doxygen

	sudo tar -xvzf doxygen-"$1".linux.bin.tar.gz -C doxygen --strip-components=1

	sudo rm -rf doxygen-"$1".linux.bin.tar.gz

	cd doxygen || exit
	cd bin || exit

	sudo mv -f doxy* /usr/bin

	cd ..
	cd ..
	sudo rm -rf doxygen
}

checkSphinx() {
	packages=("sphinx" "breathe" "sphinx-book-theme" "sphinx-copybutton" "sphinx-autobuild" "sphinx-last-updated-by-git" "sphinx-notfound-page" "sphinxcontrib-spelling" "furo" "sphinx-rtd-theme")

	all_packages_installed=true

	for package in "${packages[@]}"; do
		if ! pip show "${package}" >/dev/null 2>&1; then
			echo "${package} is not installed"
			all_packages_installed=false
		fi
	done

	if ${all_packages_installed}; then
		return 0 # All packages installed
	else
		return 1 # Some packages missing
	fi
}

setUpLCOV() {
	echo "Setting up lcov"

	curl -L -o lcov-latest.tar.gz https://github.com/linux-test-project/lcov/releases/download/v"$1"/lcov-"$1".tar.gz
	mkdir -p lcov-latest
	tar -xvzf lcov-latest.tar.gz -C lcov-latest --strip-components=1
	sudo rm -rf lcov-latest.tar.gz

	cd lcov-latest
	sudo GIT_DIR=/dev/null make install

	cd ..
	rm -rf lcov-latest

	sudo rm -rf /usr/bin/lcov
	sudo update-alternatives --install /usr/bin/lcov lcov /usr/local/bin/lcov 25
	sudo rm -rf /usr/bin/genhtml
	sudo update-alternatives --install /usr/bin/genhtml genhtml /usr/local/bin/genhtml 25
	sudo rm -rf /usr/bin/geninfo
	sudo update-alternatives --install /usr/bin/geninfo geninfo /usr/local/bin/geninfo 25
	sudo rm -rf /usr/bin/genpng
	sudo update-alternatives --install /usr/bin/genpng genpng /usr/local/bin/genpng 25
	sudo rm -rf /usr/bin/gendesc
	sudo update-alternatives --install /usr/bin/gendesc gendesc /usr/local/bin/gendesc 25
	sudo update-alternatives --install /usr/bin/perl2lcov perl2lcov /usr/local/bin/perl2lcov 25
	sudo update-alternatives --install /usr/bin/py2lcov py2lcov /usr/local/bin/py2lcov 25
	sudo update-alternatives --install /usr/bin/xml2lcov xml2lcov /usr/local/bin/xml2lcov 25
	sudo update-alternatives --install /usr/bin/xml2lcovutil.py xml2lcovutil.py /usr/local/bin/xml2lcovutil.py 25
	sudo update-alternatives --install /usr/bin/llvm2lcov llvm2lcov /usr/local/bin/llvm2lcov 25
}

installVulkan() {
	echo "Installing build essentials..."
	sudo apt-get update
	sudo apt-get install -y build-essential cmake ninja-build

	echo "Installing GLFW..."
	sudo apt-get install -y libglfw3-dev

	echo "Installing GLM..."
	sudo apt-get install -y libglm-dev

	echo "Installing tinyobjloader..."
	sudo apt-get install -y libtinyobjloader-dev || echo "tinyobjloader not found in apt, will need to be installed manually or via CMake FetchContent"

	echo "Installing stb..."
	sudo apt-get install -y libstb-dev || echo "stb not found in apt, will need to be installed manually or via CMake FetchContent"

	echo "Installing tinygltf..."
	sudo apt-get install -y libtinygltf-dev || echo "tinygltf not found in apt, will need to be installed manually or via CMake FetchContent"

	echo "Installing nlohmann-json..."
	sudo apt-get install -y nlohmann-json3-dev || echo "nlohmann-json not found in apt, will need to be installed manually or via CMake FetchContent"

	echo "Installing X Window System dependencies..."
	sudo apt-get install -y libxxf86vm-dev libxi-dev

	echo "Installing Vulkan packages..."
	sudo apt-get install -y libvulkan1

	echo "Removing any previous Vulkan SDK installation in ~/vulkansdk"
	sudo rm -rf ~/vulkansdk

	sudo mkdir -p ~/vulkansdk
	sudo wget https://sdk.lunarg.com/sdk/download/"$1"/linux/vulkansdk-linux-x86_64-"$1".tar.xz

	sudo tar -xvf vulkansdk-linux-x86_64-"$1".tar.xz -C ~/vulkansdk

	sudo rm -rf vulkansdk-linux-x86_64-"$1".tar.xz

	cd ~/vulkansdk || exit

	sudo cp -r ~/vulkansdk/"$1"/x86_64/lib/* /usr/lib/
	sudo cp -r ~/vulkansdk/"$1"/x86_64/include/* /usr/include/
	sudo cp -r ~/vulkansdk/"$1"/x86_64/bin/* /usr/bin/

	echo "Add the following to ~/.zshrc:"
	echo "source ~/vulkansdk/$1/setup-env.sh"
	echo ""
	echo "Run: source ~/.zshrc"
	echo ""
	echo "Verify installation by running: vkcube"
}

main() {
	echo "This shell file is set up to only work on Ubuntu operating systems"

	response="n"

	if [[ -z $1 ]]; then
		read -r -p "Enter (Y/N) if you are running on Ubuntu and wish to auto install all packages required: " response
	fi

	# a or 'A' for automated running (For Github workflows ignoring long documentation, linting, and formatting installation)
	if [[ ${response,,} == "y" ]] || [[ ${1,,} == "y" ]] || [[ ${response,,} == "a" ]] || [[ ${1,,} == "a" ]]; then
		echo "Update and upgrading your packages (will require an elevated user's password)"
		sudo apt-get update

		echo "Installing all the required packages for all commands used in the Makefile"

		sudo apt-get install make python3-pip docker-compose -y

		pip3 install cmake --break-system-packages

		sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 10
		sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-14 14
		sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 10

		gpp_desired_version="16.2.0"
		gpp_priority="16"
		echo "Setting up g++"

		if [[ "$(command g++ --version | grep -oP '\d+\.\d+\.\d+' || true)" == "${gpp_desired_version}" ]]; then
			echo "g++-${gpp_desired_version} exists"
		else
			setUpGCC "${gpp_desired_version}" "${gpp_priority}"
		fi

		clang_desired_version="24.0.0"
		clang_priority="24"
		echo "Setting up clang tooling"

		if [[ "$(command clang++ --version | grep -oP '\d+\.\d+\.\d+' || true)" == "${clang_desired_version}" ]]; then
			echo "clang-${clang_desired_version} exists"
		else
			setUpClang "${clang_priority}"
		fi

		checkSphinx
		status=$?

		if [[ ${status} -eq 0 ]]; then
			echo "All Sphinx packages are installed"
		else
			echo "Installing Sphinx and it's dependencies for documentation"
			sudo apt install python3-sphinx
			pip3 install --upgrade pip --break-system-packages
			sudo pip3 install sphinx breathe sphinx-book-theme sphinx-copybutton sphinx-autobuild sphinx-last-updated-by-git sphinx-notfound-page sphinxcontrib-spelling furo sphinx-rtd-theme --break-system-packages
		fi

		lcov_desired_version="2.5-0"
		lcov_priority="2.5"

		if [[ "$(command lcov --version | grep -oP '\d+\.\d+-\d+' || true)" == "${lcov_desired_version}" ]]; then
			echo "LCOV ${lcov_desired_version} exists"
		else
			setUpLCOV "${lcov_priority}"
		fi

		if [[ -x "$(command -v flawfinder || true)" ]]; then
			echo "Flawfinder already exists"
		else
			pip3 install flawfinder --break-system-packages
		fi

		# If not 'a' or 'A', set up documentation, formatting, and linting tools
		if [[ ${response,,} == "y" ]] || [[ ${1,,} == "y" ]]; then
			sudo apt-get install binutils valgrind graphviz flex bison libpcre3 libpcre3-dev lcov cppcheck xterm bear -y

			doxygen_desired_version="1.16.1"
			if [[ -x "$(command -v doxygen || true)" ]]; then
				echo "doxygen already exists"

				doxygen_version=$(doxygen --version | awk '{print $1}' || true)

				if [[ ${doxygen_version} == "${doxygen_desired_version}" ]]; then
					echo "doxygen version ${doxygen_desired_version} already exists"
				else
					installDoxygen "${doxygen_desired_version}"
				fi
			else
				installDoxygen "${doxygen_desired_version}"
			fi

			vulkan_desired_version="1.4.341.1"
			if [[ -x "$(command -v vkcube || true)" ]]; then
				echo "vkcube already exists"

				vulkan_version=$(vulkaninfo 2>/dev/null | grep -m1 'Vulkan Instance Version' | cut -d: -f2 | xargs || true)

				if [[ ${vulkan_version} == "${vulkan_desired_version}" ]]; then
					echo "vulkan version ${vulkan_desired_version} already exists"
				else
					installVulkan "${vulkan_desired_version}"
				fi
			else
				installVulkan "${vulkan_desired_version}"
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
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "compile" "" "make g++"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "release" "compile" "make"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "debug" "" "make g++"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "dev" "debug" "make"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "val" "" "make g++"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "valgrind" "val" "make valgrind"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "copy_and_run_tests" "-" "make"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "build_tests" ", copy_and_run_test" "make g++ libgmock-dev libgtest-dev (May require extra installation steps - Look at guide online)"
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
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "tracy" "" "make g++-11 gcc-11 libfreetype6-dev libcapstone-dev libegl1-mesa-dev libxkbcommon-dev libwayland-dev libdbus-1-dev libglfw3 libglfw3-dev"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "gprof" "dev" "make binutils"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "profile" "gprof" "make"
		printf "%-${col1_width}s %-${col2_width}s %-${col3_width}s\n" "initialize_repo" "-" "make git"
	fi
}

main "$@"
