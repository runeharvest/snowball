# FROM ubuntu:22.04
FROM mcr.microsoft.com/devcontainers/base:ubuntu
RUN apt update; \
    DEBIAN_FRONTEND=noninteractive \
    apt install --yes \
        autoconf \
        automake \
        bison \
        build-essential \
        ccache \
        freeglut3-dev \
        libavcodec-dev \
        libavdevice-dev \
        libavformat-dev \
        libcpptest-dev \
        libcurl4-openssl-dev \
        libfreetype6-dev \
        libgif-dev \
        libgsf-1-dev \
        libjpeg-dev \
        liblua5.2-dev \
        libluabind-dev \
        liblzma-dev \
        libmysqlclient-dev \
        libogg-dev \
        libopenal-dev \
        libpng-dev \
        libpostproc-dev \
        libsquish-dev \
        libssl-dev \
        libswscale-dev \
        libvorbis-dev \
        libxml2-dev \
        ninja-build \
        qtbase5-dev \
        qttools5-dev \
        qttools5-dev-tools \
        libqtpropertybrowser-dev \
        qt6-base-dev \
        qt6-l10n-tools \
        qt6-tools-dev \
        qt6-tools-dev-tools \
        clang-format


RUN apt install --yes \
        doxygen \
        gdb \
        git \
        graphviz \
        libxrandr-dev \
        wget

# work arround as there is no debug version available
RUN cp /lib/x86_64-linux-gnu/libcpptest.so /lib/x86_64-linux-gnu/libcpptestd.so


ARG CMAKE_VERSION=3.27.9
ARG CMAKE_INSTALL_DIR=/opt/cmake-$CMAKE_VERSION
ARG USERNAME=ryzom
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN wget --output-document=/tmp/cmake.sh https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh
RUN mkdir --parents "$CMAKE_INSTALL_DIR" \
    && sh /tmp/cmake.sh --skip-license --prefix="$CMAKE_INSTALL_DIR" \
    && ln --symbolic --force "$CMAKE_INSTALL_DIR/bin/"* /usr/local/bin


USER $USERNAME
