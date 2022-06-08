# fftune

This is an audio pitch detection library written in modern C++, providing multiple pitch detection methods and a ready to use binary to convert an input WAV file into a MIDI file.

## Building

The following libraries are required to be installed:
- [fftw3f](https://www.fftw.org/)
- [libsmf](http://libsmf.sourceforge.net/)
- [libsndfile](https://github.com/libsndfile/libsndfile)
- [sfizz](https://github.com/sfztools/sfizz)

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
# Convert audio.wav to audio.midi
podman run -v "$PWD/audio.wav:/tmp/audio.wav" fftune
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

## Tests

`fftune` ships with a testing suite, for more information checkout the [tests directory](tests/README.md).
