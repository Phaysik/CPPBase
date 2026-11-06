# Shared toolchain image for the coverage, testing, valgrind, and CodeQL workflows.
# .github/workflows/ci-image.yml tags this image with a hash of this directory,
# so editing any version below automatically produces and publishes a new image.

ARG UBUNTU_VERSION=24.04

FROM ubuntu:${UBUNTU_VERSION} AS gcc-builder

ARG GCC_VERSION=16.2.0
ARG GCC_MAJOR=16
ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        bzip2 build-essential ca-certificates curl file flex wget xz-utils \
    && rm -rf /var/lib/apt/lists/*

RUN curl -fsSL "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz" \
        | tar -xz -C /tmp \
    && cd "/tmp/gcc-${GCC_VERSION}" \
    && ./contrib/download_prerequisites \
    && mkdir /tmp/gcc-build \
    && cd /tmp/gcc-build \
    && "/tmp/gcc-${GCC_VERSION}/configure" \
        --prefix="/usr/local/gcc-${GCC_MAJOR}" \
        --enable-languages=c,c++ \
        --disable-multilib \
        --disable-bootstrap \
    && make -j"$(nproc)" \
    && make install-strip \
    && rm -rf /tmp/gcc-*

FROM ubuntu:${UBUNTU_VERSION}

ARG GCC_MAJOR=16
ARG LLVM_VERSION=24
ARG LCOV_VERSION=2.5
ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential ca-certificates curl g++-14 git gnupg gpg-agent libcapture-tiny-perl \
        libdatetime-perl libjson-xs-perl libtimedate-perl lsb-release make ninja-build \
        perl pkg-config python3-pip python3-sphinx python3-sphinx-rtd-theme \
        software-properties-common tar unzip valgrind wget xz-utils zip \
    && wget -qO /tmp/llvm.sh https://apt.llvm.org/llvm.sh \
    && chmod +x /tmp/llvm.sh \
    && /tmp/llvm.sh "${LLVM_VERSION}" \
    && rm -f /tmp/llvm.sh \
    && rm -rf /var/lib/apt/lists/* \
    && update-alternatives --install /usr/bin/clang++ clang++ "/usr/bin/clang++-${LLVM_VERSION}" "${LLVM_VERSION}" \
    && update-alternatives --install /usr/bin/clang clang "/usr/bin/clang-${LLVM_VERSION}" "${LLVM_VERSION}"

COPY --from=gcc-builder /usr/local/gcc-${GCC_MAJOR} /usr/local/gcc-${GCC_MAJOR}

# Prefer the newer libstdc++ over Ubuntu's so binaries built by GCC ${GCC_MAJOR} run.
RUN echo "/usr/local/gcc-${GCC_MAJOR}/lib64" > "/etc/ld.so.conf.d/00-gcc-${GCC_MAJOR}.conf" \
    && ldconfig \
    && for tool in gcc g++ gcov c++; do \
        update-alternatives --install "/usr/bin/${tool}" "${tool}" "/usr/local/gcc-${GCC_MAJOR}/bin/${tool}" 100; \
    done

RUN mkdir /tmp/lcov \
    && curl -fsSL "https://github.com/linux-test-project/lcov/releases/download/v${LCOV_VERSION}/lcov-${LCOV_VERSION}.tar.gz" \
        | tar -xz -C /tmp/lcov --strip-components=1 \
    && GIT_DIR=/dev/null make -C /tmp/lcov install PREFIX=/usr/local \
    && rm -rf /tmp/lcov

RUN python3 -m pip install --no-cache-dir --break-system-packages cmake

RUN printf '#include <format>\n#include <iostream>\nint main() { std::cout << std::format("{}", 42) << "\\n"; }\n' > /tmp/check.cpp \
    && g++ -std=c++26 /tmp/check.cpp -o /tmp/check-gcc \
    && /tmp/check-gcc \
    && clang++ -std=c++26 /tmp/check.cpp -o /tmp/check-clang \
    && /tmp/check-clang \
    && rm -f /tmp/check-gcc /tmp/check-clang /tmp/check.cpp \
    && g++ --version \
    && gcov --version \
    && clang++ --version \
    && cmake --version \
    && ninja --version \
    && lcov --version \
    && genhtml --version \
    && valgrind --version
