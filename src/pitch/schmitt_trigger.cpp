#include "schmitt_trigger.hpp"
#include <bits/ranges_algo.h>

namespace fftune {

schmitt_trigger::schmitt_trigger(const config &conf) {
	this->conf = conf;
}

note_estimates schmitt_trigger::detect(const sample_buffer &in) {
	/**
	 * 0.0 is the usual zero crossing
	 * 1.0 means the highest threshold, i.e. we only count it if it is the maximum magnitude
	 */
	constexpr float schmitt_threshold = 0.8;
	constexpr float step_away = 1.f - schmitt_threshold;
	note_estimates result;
	// find out boundaries of sample amplitudes
	const auto buf_view = std::span(in.data, in.size);
	const auto [min, max] = std::ranges::minmax_element(buf_view);
	const auto diff = *max - *min;
	const auto thresh_low = *min + step_away * diff;
	const auto thresh_high = *max - step_away * diff;

	size_t i = 0;

	// first skip ahead until we initially get our first Schmitt trigger switch
	while (i < in.size && in.data[i] > thresh_low && in.data[i] < thresh_high) {
		++i;
	}
	if (i >= in.size) {
		// whoopsie, looks like we don't exceed the threshold at all in the current window
		// TODO: Improvement: Instead of returning, lower the threshold and start from the beginning
		return result;
	}

	/**
	 * The current output of the Schmitt function
	 * True if high
	 * False if low
	 */
	bool schmitt_high = in.data[i] > thresh_high;
	size_t trigger_count = 0;
	for (; i < in.size; ++i) {
		const auto val = in.data[i];
		if ((schmitt_high && val < thresh_low) || (!schmitt_high && val > thresh_high)) {
			// Schmitt triggered
			++trigger_count;
			schmitt_high = !schmitt_high;
		}
	}

	const float periods = trigger_count / 2.f;
	const float freq = conf.sample_rate / (conf.buffer_size / periods);
	const auto candidate = pitch_estimate(freq);
	if (candidate.valid()) {
		result.push_back(note_estimate(candidate));
	}
	return result;
}

}
