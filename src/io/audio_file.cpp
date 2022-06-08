#include "audio_file.hpp"
#include "io/virt_file.hpp"

namespace fftune {
// TODO: Maybe allow to iterate over the audio file with a for-each loop

audio_file::audio_file() {
}

audio_file::audio_file(const std::filesystem::path &input_file) {
	open(input_file);
}

audio_file::audio_file(const int fd) {
	open(fd);
}

audio_file::audio_file(virt_file::virt_data &vio) {
	open(vio);
}

void audio_file::open(const std::filesystem::path &input_file) {
	file = SndfileHandle(input_file);
}

void audio_file::open(const int fd) {
	// we do not want libsndfile to close the file descriptor for us
	file = SndfileHandle(fd, false);
}

void audio_file::open(virt_file::virt_data &vio) {
	file = SndfileHandle(virt_file::virtio, &vio, SFM_READ, vio.raw_format, vio.raw_channels, vio.raw_samplerate);
}

bool audio_file::is_ok() const {
	return file.error() == SF_ERR_NO_ERROR;
}

std::string audio_file::error_message() const {
	return file.strError();
}

int audio_file::read(sample_buffer &buf) {
	return read(buf, buf.size);
}

int audio_file::read(sample_buffer &buf, size_t n) {
	alloc_buffer(buf.size);

	/**
	 * If we have multiple channels, we have to read all channels (frames * channels) bytes of data
	 * But we are only interested in one channel
	 *
	 * In order to simplify, we just simulate a Mono recording even for multiple-channel audio files
	 * In the future we could instead compute the mean of all channels
	 * But for now just using the first channel is fine
	 */
	const auto channels = file.channels();
	// first load into an intermediate buffer, this buffer holds all channels
	const auto result = file.read(this->buffer->data, n * channels);

	// cycle the buffer if we don't overwrite the whole buffer (simulate a ringbuffer)
	buf.cycle(n);

	// write only one channel to the target buffer
	for (size_t i = 0; i < n; ++i) {
		buf.data[i + buf.size - n] = this->buffer->data[i * channels];
	}
	return result ? n : 0;
}

float audio_file::sample_rate() const {
	return file.samplerate();
}

void audio_file::alloc_buffer(size_t num_frames) {
	const size_t size = num_frames * file.channels();
	if (buffer && size == buffer->size) {
		// don't reallocate unnecessarily
		return;
	}
	buffer = std::make_unique<sample_buffer>(size);
}

}
