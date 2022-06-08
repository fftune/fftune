#include "window.hpp"

namespace fftune {

void window::default_window(sample_buffer &data) {
	return window::templ_window<window::welch>(data);
}

void window::default_window(float *data, size_t size) {
	return window::templ_window<window::welch>(data, size);
}

float window::rectangular(const size_t i, const size_t n) {
	return 1.f;
}

float window::hanning(const size_t i, const size_t n) {
	constexpr float a0 = 0.5;
	constexpr float a1 = 1.f - a0;
	constexpr float pi2 = M_PI * 2.f;
	return a0 - a1 * std::cos(pi2 * i / n);
}

float window::hamming(const size_t i, const size_t n) {
	constexpr float a0 = 24.f/46.f;
	constexpr float a1 = 1.f - a0;
	constexpr float pi2 = M_PI * 2.f;
	return a0 - a1 * std::cos(pi2 * i / n);
}

float window::welch(const size_t i, const size_t n) {
	const float n2 = n / 2.f;
	const float q = (i - n2) / n2;
	return 1.f - q * q;
}

}
