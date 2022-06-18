#include "fftune.hpp"
#include "util/music.hpp"

namespace fftune {

bool dispatch_audio_to_midi(const std::filesystem::path &audio, const std::filesystem::path &midi, config conf) {
	switch (conf.algorithm) {
	case pitch_detection_method::Yin:
		return audio_to_midi<yin_config>(audio, midi, conf);
	case pitch_detection_method::Fast_Comb:
		return audio_to_midi<fast_comb_config>(audio, midi, conf);
	case pitch_detection_method::Fftune_Sfizz:
		return audio_to_midi<fftune_sfizz_config>(audio, midi, conf);
	case pitch_detection_method::Fftune_Spectral:
		return audio_to_midi<fftune_spectral_config>(audio, midi, conf);
	case pitch_detection_method::Double_Fft:
		return audio_to_midi<double_fft_config>(audio, midi, conf);
	default:
		return audio_to_midi<fftune_spectral_config>(audio, midi, conf);
	}
}

}
