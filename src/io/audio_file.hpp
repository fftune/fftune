#pragma once

#include <string>

#include <sndfile.hh>

#include "io/virt_file.hpp"
#include "util/music.hpp"

namespace fftune {

/**
 * @brief An audio file abstraction
 *
 * The data may be backed by either a physical file on the filesystem
 * or by a virtual file in memory
 */
class audio_file {
public:
	/**
	 * @brief Constructs an audio_file
	 *
	 * If you use this default constructor, you have to manually use open() later
	 */
	audio_file();
	/**
	 * @brief Constructs an audio_file from an on-disk input file
	 *
	 * This will open \p input_file
	 */
	explicit audio_file(const std::filesystem::path &input_file);
	/**
	 * @brief Constructs an audio_file from a file descriptor
	 *
	 * This will open \p fd which must be an already open file descriptor.
	 *
	 * The caller is responsible for closing the file descriptor again once finished.
	 */
	explicit audio_file(const int fd);
	/**
	 * @brief Constructs an audio_file from a virtual file
	 *
	 * This can be used to construct an audio_file from data only backed by physical memory.
	 * For example this can be used to open a file passed in via stdin.
	 */
	explicit audio_file(virt_file::virt_data &vio);
	/**
	 * @brief Opens a file from the filesystem
	 *
	 * This will open \p input_file
	 */
	void open(const std::filesystem::path &input_file);
	/**
	 * @brief Opens a file descriptor
	 *
	 * This will open \p fd
	 */
	void open(const int fd);
	/**
	 * @brief Opens a virtual file
	 *
	 * This will open \p vio
	 */
	void open(virt_file::virt_data &vio);
	/**
	 * @brief Reports whether the audio_file is in a sane state
	 *
	 * This will return \c true iff the backing audio file could be successfully opened
	 * and there was no error reading.
	 */
	bool is_ok() const;
	/**
	 * @brief Returns the last error as string
	 *
	 * This returns a human-readable representation of the last error
	 */
	std::string error_message() const;
	/**
	 * @brief Reads data from the input file
	 *
	 * This will read from the backing input file and store the extracted data in \p buf
	 * It will try to read as much data as \p buf can hold.
	 *
	 * It will return the amount of samples read.
	 */
	int read(sample_buffer &buf);
	/**
	 * @brief Reads \p n samples from the input file
	 *
	 * This will read \p n samples into \p buf
	 *
	 * It will return the amount of samples read.
	 */
	int read(sample_buffer &buf, size_t n);
	/**
	 * @brief Returns the sample rate of this audio_file
	 *
	 * This will return the sample rate of the backing input file.
	 */
	float sample_rate() const;
private:
	void alloc_buffer(size_t num_frames);
	SndfileHandle file;
	std::unique_ptr<sample_buffer> buffer;
};

}
