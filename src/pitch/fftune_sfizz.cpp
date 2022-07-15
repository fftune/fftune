#include "fftune_sfizz.hpp"

namespace fftune {

fftune_sfizz::fftune_sfizz(const config &conf)
	: guess_buffer(conf.buffer_size), spectrum(conf.buffer_size, conf.sample_rate) {
	this->conf = conf;
	// initialize sfizz backend
	tone_gen.init(conf);
}

note_estimates fftune_sfizz::detect(const sample_buffer &in) {
	note_estimates result;
	const float mean_rec_volume = mean_volume(in);
	const auto rec_spectrum = spectrum.detect(in);

	// iterate over voices
	for (size_t voice = 0; voice < conf.max_polyphony; ++voice) {
		std::pair<int, float> best_guess {MidiInvalid, std::numeric_limits<float>::max()};
		float confidence = 0.f;
		// iterate over all possible notes
		for (int guess = MidiMin; guess <= MidiMax; ++guess) {
			if (std::ranges::any_of(result, [&](const auto &e) { return e.note == guess; })) {
				// do not add the same note twice in a polyphonic setting
				continue;
			}
			note_estimates sounding_notes = result;
			// gather all currently pending notes plus our next guess
			sounding_notes.push_back(note_estimate(guess));
			// generate sound from our notes
			tone_gen.start(sounding_notes);
			tone_gen.gen(guess_buffer);

			/**
			 * Match the volume of the generated sound with the volume of the input waveform
			 * This is to avoid differences in volume having an effect on further steps of the algorithm
			 */
			match_volume(guess_buffer, mean_rec_volume);
			const auto guess_spectrum = spectrum.detect(guess_buffer);

			// TODO: Optimize so that we don't recompute the discrete derivative of rec_spectrum every time again
			const auto score = bins_distance(rec_spectrum, guess_spectrum);
			// did we find a better candidate?
			if (score < best_guess.second) {
				confidence = score_confidence(best_guess.second, score);
				best_guess = std::pair(guess, score);
			} else {
				// confidence can't be improved, only get lower
				confidence = std::min(confidence, score_confidence(best_guess.second, score));
			}
		}

		// check if we are confident enough
		constexpr const float confidence_threshold = 0.00f;
		if (confidence > confidence_threshold) {
			result.push_back(note_estimate(best_guess.first, volume_to_velocity(mean_rec_volume), confidence));
		}
	}
	return result;
}

float fftune_sfizz::score_confidence(const float a, const float b) {
	/**
	 * the further apart the scores are, the higher the confidence
	 * the score has to scale with buffer_size,
	 * because scores in general get larger the greater the buffer_size
	 * because the distance function has more opportunity to find different parts
	 *
	 * Accomodate for this, by scaling linearly with buffer_size
	 */
	const float score_threshold = conf.buffer_size / 4.f;
	return std::min(1.f, std::abs(a - b) / score_threshold);
}

}
