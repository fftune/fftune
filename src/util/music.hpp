#pragma once

#include <algorithm>
#include <math.h>
#include <filesystem>
#include <vector>

#include "config.hpp"
#include "sample_buffer.hpp"
#include "pitch/pitch.hpp"

namespace fftune {

void gen_sine(float freq, float sample_rate, float *buf, size_t buf_size);
void gen_harmonic(float freq0, float sample_rate, float *buf, size_t buf_size, size_t overtones = 10, float linear_dampening = 0.4);
float mean_volume(const sample_buffer &buf);
void match_volume(sample_buffer &buf, const float volume);

}
