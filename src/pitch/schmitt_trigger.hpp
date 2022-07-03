#pragma once

#include "fft/fft.hpp"

namespace fftune {

/**
 * @brief The schmitt_trigger pitch detection algorithm
 *
 * Performs pitch detection by proxying the signal through a Schmitt trigger and counting zero crossings.
 */
class schmitt_trigger {
public:
	/**
	 * @brief Constructs a schmitt_trigger object
	 *
	 * The given config \p conf will be used
	 */
	explicit schmitt_trigger(const config &conf);
	/**
	 * @brief Performs pitch detection
	 *
	 * Performs pitch detection on the input buffer \p in
	 */
	note_estimates detect(const sample_buffer &in);
private:
	config conf;
};

}
