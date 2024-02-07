FROM quay.io/centos/centos:stream9
USER root

# setup and install global dependencies and 3rd party repos and libraries
RUN dnf -y install dnf-plugins-core epel-release
RUN dnf config-manager --set-enabled crb

# install dependencies
RUN dnf -y install glm-devel libpng-devel libusb1-devel graphviz hdf5-devel libcurl-devel qwt-qt5-devel qt5-qtbase-devel libyaml-devel zlib-devel openssl-devel libxkbcommon-devel vulkan-loader-devel vulkan-validation-layers wayland-devel wayland-protocols-devel glib-devel libjpeg-turbo-devel openxr-devel vtk-devel SDL2-devel systemd systemd-devel ?? git zip unzip libtool git fmt-devel libtiff-devel boost-devel gflags-devel glog-devel patch glslang-devel protobuf-devel protobuf-compiler glfw-devel gstreamer-devel gstreamer1-plugins-base-devel libX11-xcb libxcb-devel

# post install steps
RUN sed -i 's/^[ \t]*//g' /usr/lib64/pkgconfig/*
RUN ln -s /usr/include /include
RUN git clone https://github.com/libuvc/libuvc.git
RUN cd libuvc
RUN mkdir build
RUN cd build
RUN cmake ..
RUN make && make install
RUN cd ../..
RUN rm -rf libuvc
RUN git clone https://github.com/Microsoft/vcpkg.gitcd vcpkg
RUN ./bootstrap-vcpkg.sh
RUN ./vcpkg integrate install
RUN ./vcpkg install realsense2
RUN cd ..
RUN rm -rf vcpkg

RUN useradd -ms /bin/bash illixr
USER illixr
WORKDIR /home/illixr
RUN git clone -b convertBuildToCmake https://github.com/ILLIXR/ILLIXR.git
COPY --chown=illixr cmk.sh .
RUN chmod 744 cmk.sh
