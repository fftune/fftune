#include "fft.hpp"

#include "window.hpp"

namespace fftune {

int fft_heuristic_to_flag(fft_heuristic heuristic) {
	switch (heuristic) {
	case fft_heuristic::OptimizeRuntime:
		return FFTW_MEASURE;
	case fft_heuristic::OptimizeInit:
		return FFTW_ESTIMATE;
	default:
		return FFTW_MEASURE;
	}
}


fft::fft(size_t num_samples, float sample_rate, fft_heuristic heuristic) {
	this->num_samples = num_samples;
	this->sample_rate = sample_rate;

	// in has n elements, out has n/2+1 elements
	in_buf = fftwf_alloc_real(num_samples);
	out_buf = fftwf_alloc_complex(num_samples);

	/**
	 * Init the fftw3 input buffer with 0
	 * There seems to be a very weird bug in fftw3, that causes memory corruption later on if we don't init with 0
	 * For example, fft might return only NaN
	 * The problem occurs only very indeterministically and it is never triggered if we use both FFTW_MEASURE and fftwf_cleanup()
	 * If we do not use one of the above, the problem starts occuring again
	 * I don't know why this problem exists and according to fftw3 docs, we don't need to init with 0, just allocate
	 * But I have wasted too much time debugging this mess, so just init the damn array if it takes this to workaround the issue
	 *
	 * From https://www.fftw.org/fftw3_doc/Real_002ddata-DFTs.html
	 * "The arrays need not be initialized, but they must be allocated."
	 * So I don't understand why I do have to initialize it, maybe it's just weird UB
	 */
	std::fill(in_buf, in_buf + num_samples, 0.f);

	plan = fftwf_plan_dft_r2c_1d(num_samples, in_buf, out_buf, fft_heuristic_to_flag(heuristic));
}

fft::~fft() {
	fftwf_destroy_plan(plan);

	fftwf_free(in_buf);
	fftwf_free(out_buf);
}

bins fft::detect(const sample_buffer &buf) {
	// first copy buffer so that we do not overwrite the input
	buf.write(in_buf);
	// apply windowing function
	window::default_window(in_buf, num_samples);

	fftwf_execute(plan);
	bins result;
	for (size_t i = 0; i < bins_size(); ++i) {
		result.push_back({out_buf[i][0], out_buf[i][1], i, num_samples, sample_rate});
	}
	return result;
}

size_t fft::bins_size() const {
	return num_samples / 2 + 1;
}

}
