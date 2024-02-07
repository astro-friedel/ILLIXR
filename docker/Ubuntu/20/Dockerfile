FROM ubuntu:focal
USER root

# set up environment vars
ARG DEBIAN_FRONTEND=noninteractive

# setup and install global dependencies and 3rd party repos and libraries
RUN apt-get update
RUN apt-get -y install wget software-properties-common curl apt-transport-https lsb-core
RUN add-apt-repository ppa:ecal/ecal-latest -y -u
RUN apt-get update
RUN mkdir -p /etc/apt/keyrings
RUN curl -sSf https://librealsense.intel.com/Debian/librealsense.pgp | tee /etc/apt/keyrings/librealsense.pgp > /dev/null
RUN echo "deb [signed-by=/etc/apt/keyrings/librealsense.pgp] https://librealsense.intel.com/Debian/apt-repo `lsb_release -cs` main" | tee /etc/apt/sources.list.d/librealsense.list > /dev/null
RUN more /etc/apt/sources.list.d/librealsense.list
RUN apt-get update
RUN add-apt-repository ppa:monado-xr/monado -y -u
RUN wget -nv https://monado.freedesktop.org/keys/monado-ci.asc -O /etc/apt/trusted.gpg.d/monado-ci.asc
RUN echo 'deb https://monado.pages.freedesktop.org/monado/apt focal main' | tee /etc/apt/sources.list.d/monado-ci.list
RUN apt update

# install dependencies
RUN apt-get -y install libtiff-dev zstd libusb-1.0.0-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgflags-dev libopenxr-dev libopenxr1-monado libglib2.0-dev libopencv-dev libgoogle-glog-dev glslang-dev glslang-tools libopenni2-dev libxcb-glx0-dev git libxkbcommon-dev libglfw3-dev libglm-dev qemu qemu-system qemu-system-data qemu-utils qemu-user libqcow-dev libqcow-utils libxcb-randr0-dev udev libudev-dev libwayland-dev wayland-protocols libpng-dev libprotobuf-dev libprotoc-dev protobuf-compiler libjpeg-dev libboost-all-dev libuvc-dev librealsense2-dev librealsense2-gl-dev libx11-xcb-dev patch libvulkan-dev vulkan-validationlayers ecal libsdl2-dev libfmt-dev

# post install steps
RUN wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
RUN mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600
RUN wget https://developer.download.nvidia.com/compute/cuda/11.0.3/local_installers/cuda-repo-ubuntu2004-11-0-local_11.0.3-450.51.06-1_amd64.deb
RUN dpkg -i cuda-repo-ubuntu2004-11-0-local_11.0.3-450.51.06-1_amd64.deb
RUN apt-key add /var/cuda-repo-ubuntu2004-11-0-local/7fa2af80.pub
RUN apt-get update
RUN apt-get -y install cuda
RUN rm cuda-repo-ubuntu2004-11-0-local_11.0.3-450.51.06-1_amd64.deb
RUN wget https://stereolabs.sfo2.cdn.digitaloceanspaces.com/zedsdk/4.0/ZED_SDK_Ubuntu20_cuda11.8_v4.0.8.zstd.run
RUN chmod +x ZED_SDK_Ubuntu20_cuda11.8_v4.0.8.zstd.run
RUN ./ZED_SDK_Ubuntu20_cuda11.8_v4.0.8.zstd.run -- silent
RUN rm ZED_SDK_Ubuntu20_cuda11.8_v4.0.8.zstd.run

RUN useradd -ms /bin/bash illixr
USER illixr
WORKDIR /home/illixr
RUN git clone -b convertBuildToCmake https://github.com/ILLIXR/ILLIXR.git
COPY --chown=illixr cmk.sh .
RUN chmod 744 cmk.sh
