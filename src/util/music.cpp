#include "music.hpp"

namespace fftune {

void gen_sine(float freq, float sample_rate, float *buf, size_t buf_size) {
	for (size_t i = 0; i < buf_size; ++i) {
		buf[i] = sin(freq / sample_rate * i * 2.f * std::numbers::pi);
	}
}

void gen_harmonic(float freq0, float sample_rate, float *buf, size_t buf_size, size_t overtones, float linear_dampening) {
	for (size_t i = 0; i < buf_size; ++i) {
		// add up all harmonics
		float sum = 0.f;
		for (size_t overtone = 1; overtone < overtones; ++overtone) {
			/**
			 * The first overtone is not damped at all (dampening = 1)
			 * The last overtone is damped by linear_dampening factor (dampening = linear_dampening)
			 * Interpolate inbetween
			 */
			const float dampening = 1.f - (static_cast<float>(overtone - 1) / static_cast<float>(overtones - 1) * (1.f - linear_dampening));
			sum += dampening * sin(freq0 * overtone / sample_rate * i * 2.f * std::numbers::pi);
		}
		// normalize
		sum /= (overtones - 1);
		buf[i] = sum;
	}
}

float mean_volume(const sample_buffer &buf) {
	float sum = 0.f;
	for (size_t i = 0; i < buf.size; ++i) {
		sum += std::abs(buf.data[i]);
	}
	return sum / buf.size;
}

void match_volume(sample_buffer &buf, const float volume) {
	const auto vol = mean_volume(buf);
	const auto scalar = volume / vol;
	for (size_t i = 0; i < buf.size; ++i) {
		buf.data[i] = buf.data[i] * scalar;
	}
}

}
