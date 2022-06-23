#include "pitch.hpp"

namespace fftune {

float midi_to_freq(int midi) {
	// assume equal temperament
	return FreqA4 * std::pow(2.f, (midi - MidiA4) / 12.f);
}

float freq_to_midi_float(float freq) {
	/**
	 * This computes the Midi offset (which corresponds to a semitone) from a given frequency
	 *
	 * Let s = 2^(1/12) be the semitone ratio (this is the case for equal temperament).
	 * Now obviously one can compute the frequency from a semitione offset o from A4 by computing:
	 * f = 440 * s^o
	 *
	 * Now for this function we compute the inverse by solving the same equation for o:
	 *
	 * f = 440 * s^o
	 * <=> f / 440 = s^o
	 * <=> log(f / 440) / log(s) = o
	 *
	 * Evaluating this formula and rounding gets us the nearest match.
	 */
	return MidiA4 + std::log2(freq / FreqA4) / SemitoneRatioLog;
}

int freq_to_midi(float freq) {
	return std::round(freq_to_midi_float(freq));
}

float freq_to_intonation(float freq) {
	const auto f = freq_to_midi_float(freq);
	return f - std::round(f);
}

bool freq_valid(float freq) {
	const auto midi = freq_to_midi(freq);
	return midi_valid(midi);
}

bool midi_valid(int midi) {
	return midi >= MidiMin && midi <= MidiMax;
}

float magnitude_to_velocity(float magnitude) {
	constexpr const float min_mag = -100.f;
	constexpr const float max_mag = -0.25f;
	// linearly interpolate between
	const auto factor = (max_mag - std::clamp(magnitude, min_mag, max_mag)) / (max_mag - min_mag);

	// flipped parameters, because the factor is flipped (min_mag results in highest factor)
	return std::lerp(VelocityMax, VelocityMin, factor);
}

std::string midi_to_string(int midi) {
	if (!midi_valid(midi)) {
		// not on piano
		return "Invalid Midi " + std::to_string(midi);
	}
	constexpr const std::array names = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"};
	const int octave = midi / 12.0 - 1;
	return names[midi % 12] + std::to_string(octave);
}

float wavelength_to_freq(float tau, float sample_rate) {
	return sample_rate / tau;
}

float freq_to_wavelength(float freq, float sample_rate) {
	return sample_rate / freq;
}

int volume_to_velocity(float v) {
	constexpr const float max_volume = 0.02f;
	const float scalar = std::min(v / max_volume, 1.f);
	return 60 + 60 * scalar;
}

bool evaluates_to_same_note(float f1, float f2) {
	return freq_to_midi(f1) == freq_to_midi(f2);
}


pitch_estimate::pitch_estimate(float freq, float magnitude, float confidence) {
	this->frequency = freq;
	this->magnitude = magnitude;
	this->confidence = confidence;
}

pitch_estimate::pitch_estimate(const bin &fft_bin)
	: pitch_estimate(fft_bin.frequency, fft_bin.magnitude) {
}

bool pitch_estimate::valid() const {
	return freq_valid(this->frequency);
}

std::string pitch_estimate::to_csv() const {
	return std::to_string(frequency) + "," + std::to_string(magnitude) + "," + std::to_string(confidence);
}


note_estimate::note_estimate(int note, int velocity, float confidence) {
	this->note = note;
	this->velocity = velocity;
	this->confidence = confidence;
}

note_estimate::note_estimate(pitch_estimate p) {
	this->note = freq_to_midi(p.frequency);
	this->intonation = freq_to_intonation(p.frequency);
	this->velocity = magnitude_to_velocity(p.magnitude);
}

std::string note_estimate::to_string() const {
	return midi_to_string(note);
}

bool note_estimate::valid() const {
	return midi_valid(note);
}

std::string note_estimate::to_csv() const {
	return std::to_string(note) + std::to_string(velocity) + std::to_string(confidence);
}

std::ostream &operator<<(std::ostream &os, const note_estimate &n) {
	return os << n.to_string();
}

}
