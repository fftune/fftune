#include "virt_file.hpp"

namespace fftune {

sf_count_t virt_file::filelen(void *user_data) {
	/**
	 * We don't know how long our input is yet
	 * We have to rely on libsndfile being okay with not knowing the file length
	 */
	return 0;
}

sf_count_t virt_file::seek(sf_count_t offset, int whence, void *user_data) {
	auto d = static_cast<virt_data *>(user_data);
	switch (whence) {
	case SEEK_SET:
		d->memory_head = offset;
		break;
	case SEEK_CUR:
		d->memory_head += offset;
		break;
	case SEEK_END:
		/**
		 * Seek past the end
		 * But we don't know the end, so we just go to offset
		 * If we would know the end, we would have to go to filelen + offset instead
		 */
		d->memory_head = offset;
	default:
		break;
	}
	std::cin.seekg(d->memory_head);
	return d->memory_head;
}

sf_count_t virt_file::read(void *ptr, sf_count_t count, void *user_data) {
	auto d = static_cast<virt_data *>(user_data);
	std::cin.seekg(d->memory_head);
	std::cin.read(static_cast<char *>(ptr), count);
	d->memory_head += count;
	return count;
}

sf_count_t virt_file::write(const void *ptr, sf_count_t count, void *user_data) {
	// not used
	std::cerr << "virt_file::write() called" << std::endl;
	return 0;
}

sf_count_t virt_file::tell(void *user_data) {
	auto d = static_cast<virt_data *>(user_data);
	return d->memory_head;
}

}
