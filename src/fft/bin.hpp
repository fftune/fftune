#pragma once

#include <complex>
#include <string>
#include <vector>

#include "sample_buffer.hpp"

namespace fftune {

/**
 * @brief A class representing a fft bin
 *
 * This bin holds all information necessary, most importantly the frequency and magnitude
 */
class bin {
public:
	/**
	 * @brief Constructs a bin
	 *
	 * This will initialize a bin with the given values directly
	 */
	bin(float real, float img, float magnitude = 0.f, float freq = 0.f);
	/**
	 * @brief Constructs a bin
	 *
	 * This will initialize a bin with the given values and compute frequency and magnitude
	 */
	bin(float real, float img, size_t index, size_t num_samples, float sample_rate);
	/**
	 * @brief Returns the norm
	 *
	 * This returns the norm of the bin, i.e. the magnitude of the complex value
	 */
	float norm() const;
	/**
	 * @brief Returns the phase
	 *
	 * This returns the phase of the complex value
	 */
	float phase() const;
	/**
	 * @brief Returns the real part
	 *
	 * This returns the real part of the complex value stored
	 */
	float real() const;
	/**
	 * @brief Returns the imaginary part
	 *
	 * This returns the imaginary part of the complex value stored
	 */
	float img() const;
	/**
	 * @brief The magnitude of the bin
	 *
	 * This holds the volume of the bin
	 */
	float magnitude = 0.f;
	/**
	 * @brief The frequency of the bin
	 *
	 * THis holds the frequency of the bin
	 */
	float frequency = 0.f;
private:
	std::complex<float> value;
};
/**
 * @brief Compares two bin objects
 *
 * Returns \c true iff they have the same frequency and magnitude
 */
bool operator==(const bin &l, const bin &r);

/**
 * @brief A frequency spectrum
 *
 * This is the result of a fast fourier transformation, i.e. a full frequency spectrum
 */
using bins = std::vector<bin>;
/**
 * @brief Converts a bins object to CSV
 *
 * This can be useful for debugging
 */
std::string bins_to_csv(const bins &b);
/**
 * @brief Computes the discrete derivative of a spectrum
 *
 * This returns the derivative of \p b
 */
bins discrete_derivative(const bins &b);
/**
 * @brief Computes a theoretical distance between two spectra
 *
 * This can be used to tell how similar \p a and \p b are.
 *
 * A lower value means more similar
 */
float bins_distance(const bins &a, const bins &b);

}
