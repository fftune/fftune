#include "fftune_spectral.hpp"

namespace fftune {

fftune_spectral::fftune_spectral(const config &conf)
	: spec(conf.buffer_size, conf.sample_rate) {
	this->conf = conf;
}

note_estimates fftune_spectral::detect(const sample_buffer &in) {
	note_estimates result;
	constexpr const int local_width = 5;
	pitch_estimates candidates;

	const auto spectrum = spec.detect(in);
	for (int i = 0; i < spectrum.size(); ++i) {
		const auto &candidate = spectrum[i];

		// compute local mean
		float local_mean = 0.f;
		size_t num_locals = 0;
		for (int j = std::max(i - local_width, 0); j < std::min(i + local_width, static_cast<int>(spectrum.size())); ++j) {
			local_mean += spectrum[j].magnitude;
			++num_locals;
		}
		local_mean /= num_locals;

		const auto deviation = candidate.magnitude - local_mean;
		const auto weight = deviation;

		if (weight > 0) {
			for (auto &c : candidates) {
				const auto factor = candidate.frequency / c.frequency;
				const auto drift_off = std::abs(factor - std::round(factor));
				if (drift_off < (SemitoneRatio - 1.f)) {
					// add our weight to the base pitch
					c.confidence += weight * drift_off / (SemitoneRatio - 1.f);
					;
					if (factor < 1.5f) {
						// TODO: maybe interpolate frequencies to improve accuracy
					}
				}
			}

			candidates.push_back(pitch_estimate(candidate.frequency, candidate.magnitude, weight));
		}
	}

	std::ranges::sort(candidates, [](const auto &l, const auto &r) { return l.confidence > r.confidence; });
	for (size_t i = 0; i < std::min(conf.max_polyphony, candidates.size()); ++i) {
		result.push_back(note_estimate(candidates[i]));
	}
	return result;
}

}
