#pragma once

#include "util/music.hpp"

namespace fftune {


namespace window {

typedef float(*window_f)(size_t, size_t);

template <window_f f>
void templ_window(float *data, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		const float weight = f(i, size);
		data[i] = weight * data[i];
	}
}
template <window_f f>
void templ_window(sample_buffer &data) {
	return templ_window<f>(data.data, data.size);
}

void default_window(sample_buffer &data);
void default_window(float *data, size_t size);
float rectangular(const size_t i, const size_t n);
float hanning(const size_t i, const size_t n);
float hamming(const size_t i, const size_t n);
float welch(const size_t i, const size_t n);

}
}
