#include "double_fft.hpp"

namespace fftune {

double_fft::double_fft(const config &conf)
	: spec(conf.buffer_size, conf.sample_rate) {
	this->conf = conf;
}

note_estimates double_fft::detect(const sample_buffer &in) {
	note_estimates result;
	auto spectrum = spec.detect(in);

	bins_normalize_sin(spectrum);
	// stores the second fft
	std::vector<std::pair<size_t, float>> dfft;

	for (size_t step = 1; step < spectrum.size() / 2; ++step) {
		// compute the mean
		float sum = 0.f;
		for (size_t i = 0; i < spectrum.size(); i += step) {
			sum += spectrum[i].magnitude;
		}

		dfft.push_back(std::make_pair(step, sum));
	}
	// sort by mean
	std::ranges::sort(dfft, [](const auto &l, const auto &r) { return l.second > r.second; });

	for (size_t i = 0; i < std::min(conf.max_polyphony, dfft.size()); ++i) {
		const auto b = spectrum[dfft[i].first];
		auto p = pitch_estimate(b.frequency, b.magnitude);
		result.push_back(note_estimate(p));
	}
	return result;
}

}
