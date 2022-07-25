# fftune

[![Continuous Integration](https://github.com/fftune/fftune/actions/workflows/ci.yml/badge.svg)](https://github.com/fftune/fftune/actions/workflows/ci.yml)

This is an audio pitch detection library written in modern C++20, providing multiple pitch detection methods and a ready to use binary to convert an input WAV file into a MIDI file.

## Building

The following libraries are required to be installed:
- [fftw3f](https://www.fftw.org/) - For fast Fourier transformations
- [libsmf](http://libsmf.sourceforge.net/) - For Midi file support
- [libsndfile](https://github.com/libsndfile/libsndfile) - For audio file support
- [sfizz](https://github.com/sfztools/sfizz) - For [sfz file](https://sfzformat.com/) support

`fftune` is built using `cmake`:

```bash
cmake -B build
cmake --build build

# To install
cmake --install build
```

### Dockerfile

A `Dockerfile` is provided for distributions that can not satisfy the dependency requirements.
To build the container, use `podman` or `docker`:

```bash
podman build -t fftune .
# Convert audio.wav in current directory to audio.midi
podman run -v "$PWD:/tmp/data" fftune
```

## Usage

The most basic usecase is:

```bash
audio-to-midi /path/to/audio.wav
```

It is also possible to stream data in realtime via `stdin` in an OS-independent way.
For example on Linux with Pulseaudio, you can process a live recording in realtime with:
```bash
parec --rate 48000 --format s32le --channels 1 | \
	sox -t raw -r 48000 -b 32 -e signed-integer -c 1 - -t wav - | \
	audio-to-midi --verbose -
```

There are many more options available, view them by showing the help with `audio-to-midi -h` or by looking at the provided man-page with `man audio-to-midi`.

## Documentation

This library comes with a developer API documentation. To build it use:

```bash
cd doc
doxygen
```

You can then open `doc/html/index.html` in a web browser.

If you are a developer and want to use this library in your own program, you can include the header with `#include <fftune.hpp>` and link against `libfftune.so`. Examples for using this library can be found in the [examples directory](examples/).
This library comes with `pkg-config` support, so it is not necessary to manually link against it.

## Tests

`fftune` ships with a testing suite, for more information checkout the [tests directory](tests/).
