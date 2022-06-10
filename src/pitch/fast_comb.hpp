#pragma once

#include "fft/fft.hpp"

namespace fftune {

/**
 * @brief The fast_comb pitch detection algorithm
 *
 * Performs pitch detection by sorting by the predominant peaks
 */
class fast_comb {
public:
	/**
	 * @brief Constructs a fast_comb object
	 *
	 * The given config \p conf will be used
	 */
	explicit fast_comb(const config &conf);
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
