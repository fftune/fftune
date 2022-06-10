#pragma once

#include <util/util.hpp>

namespace fftune {

/**
 * @brief An enum describing a used pitch detection method
 *
 * This enum holds all possible pitch detection algoriths
 */
enum class pitch_detection_method {
	Yin,
	Fast_Comb,
	Fftune_Spectral,
	Fftune_Sfizz,
	Invalid
};
/**
 * @brief Returns a pitch detection method from a given string
 *
 * Constructs a pitch_detection_method object from a normalized string
 */
pitch_detection_method method_from_string(const std::string &m);

/**
 * @brief An enum representing a config error
 *
 * If there is any error, this enum describes what specific error was returned
 */
enum class config_error {
	No_Error = 0,
	Hop_Mismatch,
	Externalpath_Missing,
	InvalidAlgorithm,
};
/**
 * @brief Returns whether a config_error is okay
 *
 * Returns true iff \p e is config_error::No_Error
 */
bool config_error_okay(const config_error &e);

/**
 * @brief A class representing a global pitch detection configuration
 *
 * This class is used throughout the library to bundle commonly used configuration values.
 */
class config {
public:
	/**
	 * @brief The pitch detection algorithm to be used
	 *
	 * This member determines which pitch detection method will be used
	 */
	pitch_detection_method algorithm = pitch_detection_method::Fftune_Spectral;
	/**
	 * @brief The buffer size
	 *
	 * The buffer size used for various things, such as computing the FFT or reading audio data
	 */
	size_t buffer_size = 1024;
	/**
	 * @brief The hop size
	 *
	 * The hop size determines in which substeps the read audio data will be processed.
	 * The \a buffer_size must be an integer multiple of the \a hop_size
	 */
	size_t hop_size = 1024;
	/**
	 * @brief The sample rate
	 *
	 * The sample rate being used throughout the library
	 */
	float sample_rate = 48000.f;
	/**
	 * @brief The maximum expected polyphony
	 *
	 * This determines the maximum amount of voices returned in polyphonic pitch detection.
	 * Note that it still may return less voices than this number, if not enough voices are detected.
	 */
	size_t max_polyphony = 1;
	/**
	 * @brief Whether to print verbose messages
	 *
	 * If \c true, detailed information will be printed on stdout
	 */
	bool verbose = false;
	/**
	 * @brief An external path
	 *
	 * Some pitch detection algorithms make use of an external path. In that case this path must be set to a valid value and must not go out of scope until the pitch detection method returns.
	 */
	const std::filesystem::path *external_path = nullptr;

	/**
	 * @brief Returns the error state of this config
	 *
	 * If this configuration is not valid, this function will return exact error information.
	 */
	config_error error() const;
	/**
	 * @brief Returns an error string of the last error
	 *
	 * Returns a human-readable representation of the last error.
	 */
	std::string error_str() const;
};

constexpr const config yin_config {pitch_detection_method::Yin};
constexpr const config fast_comb_config {pitch_detection_method::Fast_Comb};
constexpr const config fftune_spectral_config {pitch_detection_method::Fftune_Spectral};
constexpr const config fftune_sfizz_config {pitch_detection_method::Fftune_Sfizz};

}
