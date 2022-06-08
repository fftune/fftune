#pragma once

#include <iostream>
#include <fstream>
#include <sndfile.hh>

#include "util/util.hpp"

namespace fftune {

namespace virt_file {

/**
 * @brief Returns the file length
 *
 * This returns the file length of a virtual file.
 */
sf_count_t filelen(void *user_data);
/**
 * @brief Seeks to a position
 *
 * This seeks in the virtual file to the given position \p offset
 */
sf_count_t seek(sf_count_t offset, int whence, void *user_data);
/**
 * @brief Reads data
 *
 * This reads \p count data from the virtual file into the given buffer at \p ptr.
 * The buffer must be large enough for the requested amount of data.
 */
sf_count_t read(void *ptr, sf_count_t count, void *user_data);
/**
 * @brief Writes data
 *
 * Writes \p count data from \p ptr to the virtual file.
 */
sf_count_t write(const void *ptr, sf_count_t count, void *user_data);
/**
 * @brief Tells the offset
 *
 * This will return the current offset in the virtual file.
 */
sf_count_t tell(void *user_data);
/**
 * @brief A virtual file abstraction
 *
 * This struct holds all virtual file operations.
 */
static SF_VIRTUAL_IO virtio = {filelen, seek, read, write, tell};

/**
 * @brief A class holding data for virtual files
 *
 * This class holds all necessary data for virtual files.
 */
class virt_data {
public:
	/**
	 * @brief The current read/write head
	 *
	 * This holds the offset into the virtual file
	 */
	sf_count_t memory_head = 0;
};

}

}
