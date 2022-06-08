#pragma once

#include <vector>
#include <string>
#include <array>
#include <cmath>

namespace fftune {

constexpr const int MidiA4 = 69;
constexpr const float FreqA4 = 440.f;
constexpr const int MidiMax = 108; // highest note on piano
constexpr const int MidiMin = 21; // lowest note on piano
constexpr const int MidiInvalid = -1;
constexpr const float SemitoneRatio = 1.059463094359295; // std::pow(2.f, 1.f / 12.f)
constexpr const float SemitoneRatioLog = 0.08333333333; // std::log2(SemitoneRatio)

float freq_to_midi_float(float freq);
int freq_to_midi(float freq);
float freq_to_intonation(float freq);
bool freq_valid(float freq);
bool midi_valid(int midi);
std::string midi_to_string(int midi);
float wavelength_to_freq(float tau, float sample_rate);
float freq_to_wavelength(float freq, float sample_rate);
int volume_to_velocity(float v);


class pitch_estimate {
public:
	pitch_estimate() = default;
	explicit pitch_estimate(float freq, float magnitude = 0.f, float confidence = 1.f);
	bool valid() const;

	float frequency = 0.f;
	float magnitude = 0.f;
	float confidence = 1.f;
};

class note_estimate {
public:
	note_estimate() = default;
	explicit note_estimate(int note, int velocity = 80, float confidence = 1.f);
	explicit note_estimate(pitch_estimate p);
	std::string to_string() const;
	bool valid() const;

	int note = 0;
	int velocity = 80;
	float confidence = 1.f;
	/**
	 * @brief The intonation of this note
	 *
	 * Ranges from -0.5 to +0.5.
	 * 0.0 is perfect intonation.
	 * -0.5 is a semitone flat.
	 * +0.5 is a semitone sharp.
	 * This always assumes equal temperament.
	 */
	float intonation = 0.f;
};
std::ostream &operator<<(std::ostream &os, const note_estimate &n);
using note_estimates = std::vector<note_estimate>;

}
