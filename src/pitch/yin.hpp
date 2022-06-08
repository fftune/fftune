#pragma once

#include "util/music.hpp"

namespace fftune {

/**
 * @brief The Yin pitch detection algorithm
 *
 * This performs pitch detection on the famous Yin algorithm
 */
class yin {
public:
	/**
	 * @brief Constructs a yin object
	 *
	 * The given config \p conf will be used
	 */
	explicit yin(const config &conf);
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
