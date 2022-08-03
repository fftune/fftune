#include "fftune.hpp"

#include <getopt.h>
#include <iostream>

void show_usage() {
	std::cout << R"(Usage: wav-to-midi [-hsomvepd] /path/to/input.wav

	-h, --help		Show help
	-s, --buf-size SIZE	Change buffer and window size
	-i, --hop-size SIZE	Change the window hop size
	-o, --output FILE	Specify what file to output to
	-m, --method METHOD	Specify the algorithm to use, for possible values see the man page
	-v, --verbose		Output detailed log messages
	-e, --external-path P	Set the external path necessary for some algorithms
	-p, --polyphony NUM	Set the maximum amount of voices
	-d, --stiffness NUM 	Set the Midi stiffness

For more information visit the man page audio-to-midi(1).
)";
}

int main(int argc, char *const argv[]) {
	std::filesystem::path in_file;
	std::filesystem::path out_file;
	std::filesystem::path external_path;
	fftune::config config;
	// parse args
	constexpr struct option long_opts[] = {
		{"help", no_argument, nullptr, 'h'},
		{"buf-size", required_argument, nullptr, 's'},
		{"hop-size", required_argument, nullptr, 'i'},
		{"output", required_argument, nullptr, 'o'},
		{"method", required_argument, nullptr, 'm'},
		{"verbose", no_argument, nullptr, 'v'},
		{"external-path", required_argument, nullptr, 'e'},
		{"polyphony", required_argument, nullptr, 'p'},
		{"stiffness", required_argument, nullptr, 'd'},
		{nullptr, 0, nullptr, 0}};
	constexpr const char *short_opts = "hs:o:m:ve:p:d:";
	int opt;
	while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
		switch (opt) {
		case 'h':
			show_usage();
			return 0;
		case 's':
			config.buffer_size = atoi(optarg);
			break;
		case 'i':
			config.hop_size = atoi(optarg);
			break;
		case 'o':
			out_file = optarg;
			break;
		case 'm':
			config.algorithm = fftune::method_from_string(optarg);
			if (config.algorithm == fftune::pitch_detection_method::Invalid) {
				std::cerr << "Unknown method " << optarg << std::endl;
				return 1;
			}
			break;
		case 'v':
			config.verbose = true;
			break;
		case 'e':
			external_path = optarg;
			config.external_path = &external_path;
			break;
		case 'p':
			config.max_polyphony = atoi(optarg);
			break;
		case 'd':
			config.midi_stiffness = atoi(optarg);
			break;
		case '?':
		default:
			show_usage();
			return 1;
		}
	}

	if (optind >= argc) {
		show_usage();
		return 1;
	}

	// input file
	in_file = argv[optind];
	fftune::audio_file input_audio;
	if (in_file == "-") {
		// read from stdin
		auto vio = fftune::virt_file::virt_data(&std::cin);
		input_audio.open(vio);
	} else {
		input_audio.open(in_file);
	}

	// output file
	if (out_file.empty()) {
		out_file = in_file;
		if (out_file == "-") {
			// use fallback if input has no filename (stdin was passed)
			out_file = "stdin";
		}
		out_file.replace_extension("midi");
	}

	auto result = fftune::dispatch_audio_to_midi(in_file, out_file, config);
	return !result;
}
