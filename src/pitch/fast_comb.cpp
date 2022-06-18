#include "fast_comb.hpp"

namespace fftune {

fast_comb::fast_comb(const config &conf)
	: spec(conf.buffer_size, conf.sample_rate) {
	this->conf = conf;
}

note_estimates fast_comb::detect(const sample_buffer &in) {
	note_estimates result;
	constexpr const float magnitude_factor = 1.1f;

	auto spectrum = spec.detect(in);
	// sort peaks by magnitude
	std::ranges::sort(spectrum, [](const auto &l, const auto &r) { return l.magnitude > r.magnitude; });
	for (size_t voice = 0; voice < conf.max_polyphony; ++voice) {
		size_t best = 0;
		auto magnitude = magnitude_factor * spectrum[best].magnitude;

		// try to find a subharmonic that is loud enough to be the actual fundamental frequency
		for (size_t subharmonic_candidate = 1; subharmonic_candidate < spectrum.size(); ++subharmonic_candidate) {
			const auto candidate = spectrum[subharmonic_candidate];
			if (candidate.magnitude > magnitude) {
				// found one with high enough amplitude, but we still need to check if it is a subharmonic
				const auto factor = spectrum[best].frequency / candidate.frequency;
				const auto drift_off = std::abs(factor - std::round(factor));
				if ((drift_off < (SemitoneRatio - 1.f) / 0.25) && (factor > 1.5f)) {
					// found one!
					best = subharmonic_candidate;
					magnitude = magnitude_factor * spectrum[best].magnitude;
				}
			}
		}
		result.push_back(note_estimate(pitch_estimate(spectrum[best])));

		// subtract the amplitude from overtones
		for (size_t overtone_candidate = 0; overtone_candidate < spectrum.size(); ++overtone_candidate) {
			const auto factor = spectrum[overtone_candidate].frequency / spectrum[best].frequency;
			const auto drift_off = std::abs(factor - std::round(factor));
			if (drift_off < (SemitoneRatio - 1.f) / 0.25) {
				spectrum[overtone_candidate].magnitude -= std::abs(spectrum[best].magnitude);
			}
		}
		// we don't need this one anymore, so erase it
		spectrum.erase(spectrum.begin() + best);
		std::ranges::sort(spectrum, [](const auto &l, const auto &r) { return l.magnitude > r.magnitude; });
	}
	return result;
}

}
