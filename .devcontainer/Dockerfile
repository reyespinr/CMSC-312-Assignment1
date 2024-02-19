FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04.3

RUN apt update \
    && DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
    clang-format \
    clang-tidy \
    cmake \
    g++ \
    gcc \
    gdb \
    libgtest-dev \
    make \
    && rm -rf /var/lib/apt/lists/*