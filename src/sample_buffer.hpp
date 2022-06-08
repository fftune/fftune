#pragma once

#include <cstdlib>
#include <cstring>
#include <string>

namespace fftune {

/**
 * @brief A buffer holding audio samples
 *
 * This buffer holds audio data.
 * The buffer size must be given at creation.
 */
class sample_buffer {
public:
	sample_buffer() = delete;
	/**
	 * @brief Constructs a sample_buffer
	 *
	 * The buffer will have space according to the given \p size
	 */
	explicit sample_buffer(size_t size);
	/**
	 * @brief Destructs a sample_buffer
	 *
	 * Deallocates all data
	 */
	~sample_buffer();
	/**
	 * @brief Reads data into the buffer
	 *
	 * Reads \p n samples from the given buffer \p src into this buffer
	 */
	void read(const float *src, size_t n);
	/**
	 * @brief Reads data into the buffer
	 *
	 * Reads the entire data from the given buffer \p src into this buffer
	 */
	void read(const sample_buffer &src);
	/**
	 * @brief Writes data into another buffer
	 *
	 * Writes the entire content of this buffer into the given \p dest
	 * The given destination must be large enough to hold that data.
	 */
	void write(float *dest) const;
	/**
	 * @brief Cycles this buffer
	 *
	 * Cycles this buffer similar to a ringbuffer.
	 * All data will be shifted from the end to the start by an amount of \p n
	 */
	void cycle(size_t n);
	/**
	 * @brief Exports the buffer
	 *
	 * Returns a human-readable representation of this buffer. This is useful for debugging.
	 */
	std::string to_csv() const;
	/**
	 * @brief The backing buffer
	 *
	 * This holds the real data of this buffer
	 */
	float *data = nullptr;
	/**
	 * @brief The size of the buffer
	 *
	 * This holds the size of this buffer
	 */
	size_t size = 0;
};

}
