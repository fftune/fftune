#pragma once

#include <iostream>

#include "io/audio_file.hpp"
#include "io/midi_file.hpp"
#include "pitch/pitch_detector.hpp"

namespace fftune {

/**
 * @mainpage Introduction
 *
 * This is the documentation for the fftune library.
 *
 * This library provides pitch detection methods for automatic music transcription.
 */

// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
bool dispatch_audio_to_midi(const std::filesystem::path &audio, const std::filesystem::path &midi, config conf);

template<config T>
bool audio_to_midi(audio_file &input_file, const std::filesystem::path &midi, config conf) {
	if (!input_file.is_ok()) {
		std::cerr << "Cannot read input audio: " << input_file.error_message() << std::endl;
		return false;
	}
	const auto err = conf.error();
	if (!config_error_okay(err)) {
		std::cerr << "Invalid config: " << conf.error_str() << std::endl;
		return false;
	}
	midi_file output;

	conf.sample_rate = input_file.sample_rate();
	sample_buffer buf {conf.buffer_size};
	const double duration = conf.buffer_size / static_cast<float>(conf.sample_rate);

	/**
	 * initially read data until we almost have our first buffer ready
	 * the last part is then read in the while loop
	 *
	 * we have to additionally check if hop_size and buf_size are identical,
	 * because then read() will return 0 even for "successful" reads
	 * For our considerations a read request for 0 bytes is always successful.
	 */
	if ((buf.size != conf.hop_size) && (!input_file.read(buf, buf.size - conf.hop_size))) {
		return false;
	}

	// construct our pitch detection object
	auto p = pitch_detector<T>(conf);

	// read data in hops
	while (input_file.read(buf, conf.hop_size)) {
		auto notes = p.detect(buf);
		output.add_notes(notes, duration);

		verbose_log(notes, conf.verbose);
	}

	return output.write(midi);
}

template<config T>
bool audio_to_midi(const int input_fd, const std::filesystem::path &midi, config conf) {
	audio_file input_file {input_fd};
	return audio_to_midi<T>(input_file, midi, conf);
}

template<config T>
bool audio_to_midi(const std::filesystem::path &audio, const std::filesystem::path &midi, config conf) {
	audio_file input_file {audio};
	return audio_to_midi<T>(input_file, midi, conf);
}

}
