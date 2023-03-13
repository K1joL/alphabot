FROM ubuntu:22.04
RUN apt update \
&& apt install -y git \
&& apt install -y g++ cmake make git libgtk2.0-dev pkg-config libopencv-viz-dev\
&& git clone https://github.com/opencv/opencv.git \
&& git clone https://github.com/opencv/opencv_contrib.git \
&& mkdir -p build \
&& cd build \
&& cmake -DOPENCV_EXTRA_MODULES_PATH=opencv_contrib/modules /opencv \
&& make \
&& cd / \
&& git clone https://github.com/K1joL/alphabot \
&& cd alphabot \
&& cmake -B build \
&& cd build \
&& make \
&& ./main \