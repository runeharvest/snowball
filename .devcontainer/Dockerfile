# FROM ubuntu:22.04
FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04
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
        clang-format \
        mingw-w64 \
        mingw-w64-tools \
        libz-mingw-w64-dev


RUN apt install --yes \
        doxygen \
        gdb \
        git \
        graphviz \
        libxrandr-dev \
        wget


# RUN mkdir -p /tmp/libxml2 && \
#     wget https://gitlab.gnome.org/GNOME/libxml2/-/archive/v2.12.7/libxml2-v2.12.7.tar.gz -O /tmp/libxml2/libxml2-v2.12.7.tar.gz && \
#     cd /tmp/libxml2 && \
#     tar xf libxml2-v2.12.7.tar.gz && \
#     cd libxml2-v2.12.7 && \
#     ./autogen.sh && \
#     ./configure --without-python --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32 && \
#     make && \
#     make install

# RUN mkdir -p /tmp/libpng && \
#     wget -O /tmp/libpng/libpng-1.6.43.tar.gz https://download.sourceforge.net/libpng/libpng-1.6.43.tar.gz && \
#     cd /tmp/libpng && \
#     tar xf libpng-1.6.43.tar.gz && \
#     cd libpng-1.6.43 && \
#     ./configure --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32 && \
#     make && \
#     sudo make install

# RUN mkdir -p /tmp/openssl && \
#     wget -O /tmp/openssl/openssl-3.0.13.tar.gz https://www.openssl.org/source/openssl-3.0.13.tar.gz && \
#     cd /tmp/openssl && \
#     tar xf openssl-3.0.13.tar.gz && \
#     cd openssl-3.0.13 && \
#     ./Configure mingw64 no-shared --cross-compile-prefix=x86_64-w64-mingw32- --prefix=/usr/x86_64-w64-mingw32 && \
#     make && \
#     make install_sw

# RUN mkdir -p /tmp/boost && \
#     wget -O /tmp/boost/boost_1_84_0.tar.gz https://archives.boost.io/release/1.84.0/source/boost_1_84_0.tar.bz2 && \
#     cd /tmp/boost && \
#     tar xf boost_1_84_0.tar.gz && \
#     cd boost_1_84_0 && \
#     cp -r boost /usr/x86_64-w64-mingw32/include/

# RUN mkdir -p /tmp/lua-5.1 && \
#     cd /tmp/lua-5.1 && \
#     curl -LO https://www.lua.org/ftp/lua-5.1.5.tar.gz && \
#     tar xzf lua-5.1.5.tar.gz && \
#     cd lua-5.1.5 && \
#     make mingw \
#         CC=x86_64-w64-mingw32-gcc \
#         AR="x86_64-w64-mingw32-ar rcu" \
#         RANLIB=x86_64-w64-mingw32-ranlib && \
#     make mingw install \
#         CC=x86_64-w64-mingw32-gcc \
#         AR="x86_64-w64-mingw32-ar rcu" \
#         RANLIB=x86_64-w64-mingw32-ranlib \
#         INSTALL_TOP=/usr/x86_64-w64-mingw32 && \
#     make install \
#         INSTALL_TOP=/usr/x86_64-w64-mingw32 \
#         TO_BIN="lua.exe luac.exe"


# RUN mkdir -p /tmp/luabind && \
#     git clone https://github.com/oberon00/luabind.git /tmp/luabind/luabind && \
#     cd /tmp/luabind/luabind && \
#     mkdir -p build && cd build && \
#     cmake .. \
#     -DCMAKE_TOOLCHAIN_FILE=/src/cmake/toolchain-mingw32.cmake \
#     -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
#     -DLUA_INCLUDE_DIR=/usr/x86_64-w64-mingw32/include \
#     -DLUA_LIBRARY=/usr/x86_64-w64-mingw32/lib/liblua51.a && \
#     make -j && \
#     sudo make install


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
