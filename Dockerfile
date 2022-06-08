FROM archlinux:base

# default compiler
ARG cxx=g++
ENV CXX=$cxx

RUN pacman -Syu --noconfirm git gcc clang cmake ninja pkgconf doxygen gtest sfizz fftw libsmf libsndfile
COPY . /tmp/fftune
WORKDIR /tmp/fftune
RUN scripts/build.sh
CMD audio-to-midi /tmp/audio.wav
