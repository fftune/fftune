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
