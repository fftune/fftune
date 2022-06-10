#pragma once

#include "fft/fft.hpp"

namespace fftune {

/**
 * @brief The fftune_spectral pitch detection algorithm
 *
 * Performs pitch detection by detecting peaks in the frequency spectrum.
 */
class fftune_spectral {
public:
	/**
	 * @brief Constructs a fftune_spectral object
	 *
	 * The given config \p conf will be used
	 */
	explicit fftune_spectral(const config &conf);
	/**
	 * @brief Performs pitch detection
	 *
	 * Performs pitch detection on the input buffer \p in
	 */
	note_estimates detect(const sample_buffer &in);
private:
	config conf;
	fft spec;
};

}
