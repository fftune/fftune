#include "tone_generator.hpp"

// sfizz seems to only work with this hardcoded value
// we can merge multiple blocks together afterwards to provide a different buffersize for FFT application
#define SFIZZ_BUFFERSIZE 1024ul

namespace fftune {

tone_generator::tone_generator() {
}

tone_generator::~tone_generator() {
	free(left_out);
	free(right_out);
}

bool tone_generator::init(const config &conf) {
	return init(conf.buffer_size, conf.sample_rate, *conf.external_path);
}

bool tone_generator::init(size_t buffer_size, float sample_rate, std::filesystem::path sfz) {
	this->buffer_size = buffer_size;

	sfizz.setSamplesPerBlock(SFIZZ_BUFFERSIZE);
	sfizz.setSampleRate(sample_rate);

	/**
	 * Note that we allocate buffers for the whole buffer_size
	 * Right now this is not really necessary as we only need SFIZZ_BUFFERSIZE amount of memory
	 * But maybe we fix this SFIZZ_BUFFERSIZE bug in the future, so then we don't have to write back in multiple passes
	 * Until then we really only use the lowest SFIZZ_BUFFERSIZE bytes of the buffers
	 */
	left_out = (float *) malloc(sizeof(float) * std::max(buffer_size, SFIZZ_BUFFERSIZE));
	right_out = (float *) malloc(sizeof(float) * std::max(buffer_size, SFIZZ_BUFFERSIZE));
	std::fill(left_out, left_out + buffer_size, 0.f);
	std::fill(right_out, right_out + buffer_size, 0.f);

	return sfizz.loadSfzFile(sfz);
}

void tone_generator::gen(sample_buffer &out) {
	// note that we assume that out has the same size as buffer_size passed in init()
	float *stereo_out[] = {left_out, right_out};

	for (size_t k = 0; k < buffer_size / SFIZZ_BUFFERSIZE; ++k) {
		sfizz.renderBlock(stereo_out, SFIZZ_BUFFERSIZE, 1);
		// write the data correctly padded to the output buffer
		for (int i = 0; i < SFIZZ_BUFFERSIZE; ++i) {
			// we simply just use the left channel, but we could also merge channels in the future
			out.data[k * SFIZZ_BUFFERSIZE + i] = left_out[i];
		}
	}
}

void tone_generator::start(const std::vector<note_estimate> &midis, size_t offset) {
	float *stereo_out[] = {left_out, right_out};

	// midi reset
	sfizz.allSoundOff();
	// activate all passed notes
	std::ranges::for_each(midis, [&](const auto &midi) { sfizz.noteOn(0, midi.note, midi.velocity); });

	// wait for the attack phase of the note to decay
	// this allows us to get a more pitch consistent sample
	for (int i = 0; i < offset - 1; ++i) {
		sfizz.renderBlock(stereo_out, SFIZZ_BUFFERSIZE, 1);
	}
}

}
