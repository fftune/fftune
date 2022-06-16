#include "audio_file.hpp"
#include "io/virt_file.hpp"

namespace fftune {

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
	return file.error() == SF_ERR_NO_ERROR && !at_end;
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

	if (result) {
		return n;
	} else {
		at_end = true;
		return 0;
	}
}

float audio_file::sample_rate() const {
	return file.samplerate();
}


audio_file::iterator::iterator(audio_file *audio, sample_buffer *buf, size_t hop_size) {
	this->audio = audio;
	this->buf = buf;
	this->hop_size = hop_size;
}

bool audio_file::iterator::operator!=(iterator r) {
	if (r.audio == audio) {
		// if both audio backend, then they are equal if they are both nullptr
		return audio != nullptr;
	} else if (r.audio == nullptr) {
		// if audio backends are different, then they are only equal if one is nullptr and the other is not okay
		return audio->is_ok();
	} else if (audio == nullptr) {
		return r.audio->is_ok();
	}
	return true;
}

sample_buffer *audio_file::iterator::operator*() {
	return this->buf;
}

void audio_file::iterator::operator++() {
	if (!audio) {
		return;
	}
	audio->read(*buf, hop_size);
}

audio_file::view::view(audio_file *audio, sample_buffer *buf, size_t hop_size)
	: audio(audio), buf(buf), hop_size(hop_size) {
}

audio_file::iterator audio_file::view::begin() {
	auto result = audio_file::iterator(audio, buf, hop_size);
	++result;
	return result;
}

audio_file::iterator audio_file::view::end() {
	return audio_file::iterator(nullptr, nullptr, 0);
}

audio_file::view audio_file::iter(sample_buffer *buf, size_t hop_size) {
	return audio_file::view(this, buf, hop_size);
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
