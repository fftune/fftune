#include "yin.hpp"

namespace fftune {

yin::yin(const config &conf) {
	this->conf = conf;
}

note_estimates yin::detect(const sample_buffer &in) {
	note_estimates result;
	constexpr const float threshold = 0.1f;
	// We don't need to recompute the mean every iteration
	float cumulative_mean = 0.f;
	for (size_t tau = 1; tau < conf.buffer_size; ++tau) {
		float sum = 0.f;
		for (size_t j = 0; j < conf.buffer_size - tau; ++j) {
			sum += squared(in.data[j] - in.data[j + tau]);
		}
		cumulative_mean += sum;
		if (sum / (cumulative_mean / static_cast<float>(tau)) < threshold) {
			// found a peak
			const float freq = wavelength_to_freq(tau, conf.sample_rate);
			pitch_estimate candidate {freq};

			/**
			 * Only add the candidate if it is reasonable, i.e. within some reasonable frequency bounds
			 * If it is not valid, we right now return an empty list
			 * An alternative would be to continue searching for better candidates
			 * But heuristically, continueing the search is probably not very fruitful
			 * given that we found an invalid candidate, that is not even a note on the piano
			 */
			if (candidate.valid()) {
				result.push_back(note_estimate(candidate));
			}
			break;
		}
	}
	return result;
}

}
