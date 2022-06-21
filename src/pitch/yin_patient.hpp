#pragma once

#include "util/music.hpp"

namespace fftune {

/**
 * @brief The Yin Patient pitch detection algorithm
 *
 * This is a variant on the Yin pitch detection algorithm.
 */
class yin_patient {
public:
	/**
	 * @brief Constructs a yin_patient object
	 *
	 * The given config \p conf will be used
	 */
	explicit yin_patient(const config &conf);
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
