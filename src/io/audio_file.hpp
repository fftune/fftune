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


	/**
	 * @brief An iterator over an audio_file
	 *
	 * This class can be used to iterate over an audio_file in a for-each loop
	 */
	class iterator {
	public:
		/**
		 * @brief Constructs an iterator
		 *
		 * This constructs an iterator for the given \p audio filee
		 */
		iterator(audio_file *audio, sample_buffer *buf, size_t hop_size);
		/**
		 * @brief The backing audio_file
		 *
		 * This member holds the backing audio_file to read from.
		 */
		audio_file *audio = nullptr;
		/**
		 * @brief The backing buffer
		 *
		 * This member holds the backing sample_buffer to read into.
		 */
		sample_buffer *buf = nullptr;
		/**
		 * @brief The hop size
		 *
		 * This member holds the amount of data to read in each iteration
		 */
		size_t hop_size = 0;

		/**
		 * @brief Decides if two iterators are not equal
		 *
		 * This is used to enable automatic for-each loops
		 */
		bool operator!=(iterator r);
		/**
		 * @brief Returns the underlying data
		 *
		 * This returns the backing sample_buffer
		 */
		sample_buffer *operator*();
		/**
		 * @brief Increments the iterator
		 *
		 * This will read data from the backing audio_file again
		 * and return a fresh sample_buffer with the new data.
		 */
		void operator++();
	};

	/**
	 * @brief A view over an audio_file
	 *
	 * This view can automatically create an audio_file::iterator
	 */
	class view {
	public:
		/**
		 * @brief Constructs a view
		 *
		 * This constructs a view for the given \p buf and \p hop_size
		 */
		view(audio_file *audio, sample_buffer *buf, size_t hop_size);
		/**
		 * @brief The beginning iterator of the view
		 *
		 * This iterator is the start of the view and has already one iteration of samples read from the audio_file
		 */
		iterator begin();
		/**
		 * @brief The end iterator of the view
		 *
		 * This iterator denotes the end of the view
		 */
		iterator end();
	private:
		audio_file *audio;
		sample_buffer *buf;
		size_t hop_size = 0;
	};

	/**
	 * @brief Creates an audio_file::view
	 *
	 * This creates an iteratable audio_file::view for the given \p hop_size
	 */
	view iter(sample_buffer *buf, size_t hop_size);
private:
	void alloc_buffer(size_t num_frames);
	SndfileHandle file;
	std::unique_ptr<sample_buffer> buffer;
	bool at_end = false;
};

}
