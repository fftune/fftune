#include "yin_patient.hpp"

namespace fftune {

yin_patient::yin_patient(const config &conf) {
	this->conf = conf;
}

note_estimates yin_patient::detect(const sample_buffer &in) {
	note_estimates result;
	pitch_estimates candidates;
	constexpr const float threshold = 0.1f;

	// We don't need to recompute the mean every iteration
	float cumulative_mean = 0.f;
	for (size_t tau = 1; tau < conf.buffer_size; ++tau) {
		float sum = 0.f;
		for (size_t j = 0; j < conf.buffer_size - tau; ++j) {
			sum += squared(in.data[j] - in.data[j + tau]);
		}
		cumulative_mean += sum;

		const float freq = wavelength_to_freq(tau, conf.sample_rate);
		auto candidate = pitch_estimate(freq);
		candidate.confidence = 1 - (sum / (cumulative_mean / static_cast<float>(tau)));

		if (candidate.valid()) {
			// check if the candidate was already added
			auto c = std::ranges::find_if(candidates, [&](const auto &e) { return evaluates_to_same_note(candidate.frequency, e.frequency); });
			if (c == candidates.end()) {
				// not found, just add it then
				candidates.push_back(candidate);
			} else {
				// lost and found, see if we can improve it then
				if (candidate.confidence > c->confidence) {
					c->frequency = candidate.frequency;
					c->confidence = candidate.confidence;
				}
			}
		}
	}

	std::ranges::sort(candidates, [](const auto &l, const auto &r) { return l.confidence > r.confidence; });
	for (size_t voice = 0; voice < conf.max_polyphony && !candidates.empty(); ++voice) {
		// the first one is the winner!
		result.push_back(note_estimate(candidates[0]));

		// now give a penalty to all harmonics of the winner
		for (size_t i = 1; i < candidates.size(); ++i) {
			const auto factor = std::max(candidates[0].frequency, candidates[i].frequency) / std::min(candidates[0].frequency, candidates[i].frequency);
			const auto drift_off = std::abs(factor - std::round(factor));
			if (drift_off < 0.5 * (SemitoneRatio - 1.f)) {
				candidates[i].confidence -= std::abs(candidates[0].confidence);
			}
		}

		// remove the winner, you can't be elected twice, this isn't a presidency election
		candidates.erase(candidates.begin());
		// we have to resort now
		std::ranges::sort(candidates, [](const auto &l, const auto &r) { return l.confidence > r.confidence; });
	}
	return result;
}

}
