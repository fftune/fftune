#pragma once

#include "fft/fft.hpp"

namespace fftune {

/**
 * @brief The double_fft pitch detection algorithm
 *
 * Performs pitch detection by applying something similar to a second FFT to the frequency spectrum
 */
class double_fft {
public:
	/**
	 * @brief Constructs a double_fft object
	 *
	 * The given config \p conf will be used
	 */
	explicit double_fft(const config &conf);
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
