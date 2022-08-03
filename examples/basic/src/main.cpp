#include <fftune.hpp>

int main(int argc, char *argv[]) {
	const float freq = 440.f; // frequency of A4
	fftune::config config;
	fftune::sample_buffer buf {config.buffer_size};
	fftune::pitch_detector<fftune::yin_config> p {config};

	// generate a harmonic signal
	fftune::gen_harmonic(freq, config.sample_rate, buf.data, buf.size);
	// detect notes
	const auto result = p.detect(buf);
	if (result.empty()) {
		std::cerr << "Failed to detect a note" << std::endl;
		return 1;
	}
	// this should output the correct note A4
	std::cout << result.front().to_string() << std::endl;
	return 0;
}
