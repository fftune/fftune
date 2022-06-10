#pragma once

#include <cstring>
#include <vector>

#include <fftw3.h>

#include "bin.hpp"

namespace fftune {

/**
 * @brief A heuristic to be used for FFT application
 *
 * This describes what part of the FFT to optimize for.
 */
enum class fft_heuristic {
	OptimizeRuntime,
	OptimizeInit,
};
/**
 * @brief Converts a heuristic to a fftw compatible flag
 *
 * Returns a flag, that can be used when creating a fftw plan.
 */
int fft_heuristic_to_flag(fft_heuristic heuristic);

/**
 * @brief A fast fourier transformation
 *
 * This object can perform a FFT. The buffer size and sample rate must be given at creation.
 */
class fft {
public:
	fft() = delete;
	/**
	 * @brief Constructs a fft object
	 *
	 * The buffer size is set according to \p num_samples, the sample rate is given via \p sample_rate
	 */
	fft(size_t num_samples, float sample_rate, fft_heuristic heuristic = fft_heuristic::OptimizeRuntime);
	/**
	 * @brief Destructs a fft object
	 *
	 * This destroys the plan, but note that fftw may be caching the plan for future use.
	 */
	~fft();
	/**
	 * @brief Performs a FFT on a given buffer
	 *
	 * Reads samples from \p buf and returns the result of the FFT.
	 * Note that \p buf must be large enough to hold \a num_samples samples
	 */
	bins detect(const sample_buffer &buf);
	/**
	 * @brief Returns the size of the bins returned from a FFT
	 *
	 * This is equal to \f$\frac{n}{2} + 1\f$, where \c n is \a num_samples
	 */
	size_t bins_size() const;
private:
	size_t num_samples;
	float sample_rate;
	float *in_buf = nullptr;
	fftwf_complex *out_buf = nullptr;
	fftwf_plan plan;
};

}
