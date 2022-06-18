#include "config.hpp"

namespace fftune {

pitch_detection_method method_from_string(const std::string &m) {
	const auto canonical = to_lower(m);
	if (canonical == "fftune-spectral") {
		return pitch_detection_method::Fftune_Spectral;
	} else if (canonical == "fftune-sfizz") {
		return pitch_detection_method::Fftune_Sfizz;
	} else if (canonical == "yin") {
		return pitch_detection_method::Yin;
	} else if (canonical == "fast-comb") {
		return pitch_detection_method::Fast_Comb;
	} else if (canonical == "double-fft") {
		return pitch_detection_method::Double_Fft;
	} else {
		return pitch_detection_method::Invalid;
	}
}

bool config_error_okay(const config_error &e) {
	return e == config_error::No_Error;
}

config_error config::error() const {
	config_error result = config_error::No_Error;

	const float hop_factor = static_cast<float>(buffer_size) / static_cast<float>(hop_size);
	if (hop_factor < 1.0f || !is_int(hop_factor)) {
		result = config_error::Hop_Mismatch;
	} else if (algorithm == pitch_detection_method::Fftune_Sfizz && !external_path) {
		result = config_error::Externalpath_Missing;
	} else if (algorithm == pitch_detection_method::Invalid) {
		result = config_error::InvalidAlgorithm;
	}

	return result;
}

std::string config::error_str() const {
	const auto err = error();
	switch (err) {
	case config_error::No_Error:
		return "";
	case config_error::Hop_Mismatch:
		return "Invalid hop size. The buffer size must be an integer multiple of the hop size.";
	case config_error::Externalpath_Missing:
		return "This pitch detection method requires the external path to be set.";
	case config_error::InvalidAlgorithm:
		return "Invalid algorithm chosen.";
	default:
		return "Config error";
	}
}

}
