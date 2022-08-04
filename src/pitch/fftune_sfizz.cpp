#include "fftune_sfizz.hpp"

namespace fftune {

fftune_sfizz::fftune_sfizz(const config &conf)
	: guess_buffer(conf.buffer_size), spectrum(conf.buffer_size, conf.sample_rate) {
	this->conf = conf;
	// initialize sfizz backend
	tone_gen.init(conf);
}

note_estimates fftune_sfizz::detect(const sample_buffer &in) {
	note_estimates sounding_notes;
	const float mean_rec_volume = mean_volume(in);
	const auto rec_spectrum = spectrum.detect(in);

	const int max_id = power(MidiRange, conf.max_polyphony);
	std::pair<int, float> best_guess {MidiInvalid, std::numeric_limits<float>::max()};
	float confidence = 0.f;

	// iterate over all possible notes
	for (int id = 0; id < max_id; ++id) {
		sounding_notes.clear();
		// gather all currently pending notes
		this->add_notes(sounding_notes, id);
		// generate sound from our notes
		tone_gen.start(sounding_notes);
		tone_gen.gen(guess_buffer);

		/**
		 * Match the volume of the generated sound with the volume of the input waveform
		 * This is to avoid differences in volume having an effect on further steps of the algorithm
		 */
		match_volume(guess_buffer, mean_rec_volume);
		const auto guess_spectrum = spectrum.detect(guess_buffer);

		// evaluate similarity with a spectral difference function
		const auto score = bins_distance_complete(rec_spectrum, guess_spectrum);
		// did we find a better candidate?
		if (score < best_guess.second) {
			confidence = score_confidence(best_guess.second, score);
			best_guess = std::pair(id, score);
		} else {
			// confidence can't be improved, only get lower
			confidence = std::min(confidence, score_confidence(best_guess.second, score));
		}
	}

	sounding_notes.clear();
	// check if we are confident enough
	constexpr const float confidence_threshold = 0.00f;
	if (confidence > confidence_threshold) {
		// reload the best guess
		this->add_notes(sounding_notes, best_guess.first);

		for (auto &n : sounding_notes) {
			n.velocity = volume_to_velocity(mean_rec_volume);
			n.confidence = confidence;
		}
	}

	return sounding_notes;
}

void fftune_sfizz::add_notes(note_estimates &notes, int id) {
	// iterate over all voices
	for (size_t voice = 0; voice < conf.max_polyphony; ++voice) {
		/**
		 * This is a clever hack to encode all notes in a single integer
		 * Here we do the decoding via modulo.
		 *
		 * The reason that we do this is to avoid manually nesting for loops for polyphonic support
		 * Instead we can iterate with a single for loop over all voices simultaneously,
		 * because we encode all voices in a single variable.
		 */
		const int note = MidiMin + id % MidiRange;

		// do not add the same note twice in a polyphonic setting
		if (!std::ranges::any_of(notes, [&](const auto &e) { return e.note == note; })) {
			notes.push_back(note_estimate(note));
		}

		id /= MidiRange;
	}
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
