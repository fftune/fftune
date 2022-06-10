#pragma once

#include <filesystem>

#ifdef HAS_SFIZZ
#include <sfizz.hpp>
#endif

#include "util/music.hpp"

namespace fftune {

/**
 * @brief An abstraction for generating tones
 *
 * This class can generate tones with a sfizz backend.
 */
class tone_generator {
public:
	tone_generator();
	~tone_generator();
	/**
	 * @brief Initializes the tone_generator instance
	 *
	 * This initializes this class with the given config \p conf
	 */
	bool init(const config &conf);
	/**
	 * @brief Initializes the tone_generator instance
	 *
	 * This initializes this class with the given \p buffer_size, \p sample_rate and \p sfz path
	 */
	bool init(size_t buffer_size, float sample_rate, std::filesystem::path sfz);
	/**
	 * @brief Generate audio
	 *
	 * This generates samples and writes them to the passed buffer \p out
	 */
	void gen(sample_buffer &out);
	/**
	 * @brief Activate notes
	 *
	 * This activates the notes given via \p midis, effectively sending noteon Midi events.
	 */
	void start(const std::vector<note_estimate> &midis, size_t offset = 7);
private:
	float *left_out = nullptr;
	float *right_out = nullptr;
	size_t buffer_size;
#ifdef HAS_SFIZZ
	sfz::Sfizz sfizz;
#endif
};

}
