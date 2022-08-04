#pragma once

#include "fft/fft.hpp"
#include "tone_generator.hpp"

namespace fftune {

/**
 * @brief A pitch detection algorithm using sfizz
 *
 * This algorithm uses the sfizz library to create audio and compares the input sound and the created sound in the frequency spectrum to determine how close they are.
 * It then picks the closest note with regard to that distance function.
 */
class fftune_sfizz {
public:
	/**
	 * @brief Constructs a fftune_sfizz
	 *
	 * The given config \p conf will be used
	 */
	explicit fftune_sfizz(const config &conf);
	/**
	 * @brief Performs pitch detection
	 *
	 * This performs pitch detection on the input buffer \p in
	 */
	note_estimates detect(const sample_buffer &in);
private:
	void add_notes(note_estimates &notes, int id);
	float score_confidence(const float a, const float b);
	config conf;
	tone_generator tone_gen;
	sample_buffer guess_buffer;
	fft spectrum;
};

}
