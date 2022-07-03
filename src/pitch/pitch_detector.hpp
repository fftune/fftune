#pragma once

#include "double_fft.hpp"
#include "fast_comb.hpp"
#include "fftune_sfizz.hpp"
#include "fftune_spectral.hpp"
#include "schmitt_trigger.hpp"
#include "yin.hpp"
#include "yin_patient.hpp"

namespace fftune {

/**
 * @brief A generic pitch detector
 *
 * This class is an abstraction over all available pitch detection algorithms
 * It is a zero-cost abstraction, since the backend can be chosen at compile time,
 * therefore the compiler can eliminate the indirection at compile time.
 *
 * Object orientation with inheritance + vtable is intentionally avoided here,
 * but if you wish to choose a pitch detection backend at runtime, checkout fftune::dispatch_audio_to_midi()
 */
template<config T>
class pitch_detector {
public:
	/**
	 * @brief Constructs a pitch_detector
	 *
	 * The \p conf parameter is another config than the templated config.
	 * The templated config decides which pitch detection backend is used, at compile time.
	 * The parameter config \p conf is passed to the underlying pitch detection backend at runtime.
	 */
	explicit pitch_detector(config conf)
		: method(conf) {
	}
	/**
	 * @brief The pitch detection backend
	 *
	 * This member holds the correct pitch detection backend, and must be chosen at compile time.
	 */
	typename std::conditional<T.algorithm == pitch_detection_method::Yin, yin, typename std::conditional<T.algorithm == pitch_detection_method::Fftune_Sfizz, fftune_sfizz, typename std::conditional<T.algorithm == pitch_detection_method::Fftune_Spectral, fftune_spectral, typename std::conditional<T.algorithm == pitch_detection_method::Fftune_Spectral, fast_comb, typename std::conditional<T.algorithm == pitch_detection_method::Double_Fft, double_fft, typename std::conditional<T.algorithm == pitch_detection_method::Yin_Patient, yin_patient, schmitt_trigger>::type>::type>::type>::type>::type>::type method {T};
	/**
	 * @brief Performs pitch detection
	 *
	 * This calls the pitch detection method of the chosen backend for the input buffer \p in
	 */
	note_estimates detect(const sample_buffer &in) {
		return method.detect(in);
	}
};

}
