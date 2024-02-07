FROM fedora:37
USER root

# install dependencies
RUN dnf -y install glm-devel libpng-devel libusb1-devel openni-devel graphviz hdf5-devel libcurl-devel qwt-qt5-devel qt5-qtbase-devel libyaml-devel zlib-devel openssl-devel libxkbcommon-devel vulkan-loader-devel vulkan-validation-layers wayland-devel wayland-protocols-devel glib-devel libjpeg-turbo-devel openxr-devel SDL2-devel ?? librealsense-devel git fmt-devel systemd-udev systemd-devel libtiff-devel boost-devel gflags-devel glog-devel patch glslang-devel protobuf-devel protobuf-compiler glfw-devel gstreamer-devel gstreamer1-plugins-base-devel libX11-xcb libxcb-devel opencv-devel

# post install steps
RUN sed -i 's/^[ 	]*//g' /usr/lib64/pkgconfig/*
RUN ln -s /usr/include /include
RUN git clone https://github.com/libuvc/libuvc.git
RUN cd libuvc
RUN mkdir build
RUN cd build
RUN cmake ..
RUN make && make install
RUN cd ../..
RUN rm -rf libuvc

RUN useradd -ms /bin/bash illixr
USER illixr
WORKDIR /home/illixr
RUN git clone -b convertBuildToCmake https://github.com/ILLIXR/ILLIXR.git
COPY --chown=illixr cmk.sh .
RUN chmod 744 cmk.sh
