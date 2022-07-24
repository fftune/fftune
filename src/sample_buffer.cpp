#include "sample_buffer.hpp"

namespace fftune {

sample_buffer::sample_buffer(size_t size) {
	data = static_cast<float *>(malloc(sizeof(float) * size));
	this->size = size;
}

sample_buffer::~sample_buffer() {
	free(data);
}

void sample_buffer::read(const float *src, size_t n) {
	// if n is less than size, we simulate a ringbuffer
	cycle(n);
	// copy the newly read samples to the end
	std::memcpy(this->data + this->size - n, src, sizeof(float) * n);
}

void sample_buffer::read(const sample_buffer &src) {
	read(src.data, src.size);
}

void sample_buffer::write(float *dest) const {
	std::memcpy(dest, this->data, this->size * sizeof(float));
}

void sample_buffer::cycle(size_t n) {
	// rotate the buffer
	std::memmove(this->data, this->data + n, (this->size - n) * sizeof(float));
}

std::string sample_buffer::to_csv() const {
	std::string result;
	for (size_t i = 0; i < this->size; ++i) {
		result.append(std::to_string(this->data[i]) + "\n");
	}
	return result;
}

}
