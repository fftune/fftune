FROM archlinux:base

# default compiler
ARG cxx=g++
ENV CXX=$cxx

RUN pacman -Syu --noconfirm git gcc clang cmake ninja pkgconf doxygen gtest sfizz fftw libsmf libsndfile
RUN git clone https://github.com/fftune/fftune /tmp/fftune
WORKDIR /tmp/fftune
RUN scripts/build.sh
CMD audio-to-midi /tmp/data/audio.wav
