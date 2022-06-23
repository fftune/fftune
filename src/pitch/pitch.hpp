#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "fft/bin.hpp"

namespace fftune {

constexpr const int MidiA4 = 69;
constexpr const float FreqA4 = 440.f;
constexpr const int MidiMax = 108; // highest note on piano
constexpr const int MidiMin = 21; // lowest note on piano
constexpr const int MidiInvalid = -1;
constexpr const int VelocityMax = 127; // maxium Midi velocity
constexpr const int VelocityMin = 0; // minimum Midi velocity
constexpr const float SemitoneRatio = 1.059463094359295; // std::pow(2.f, 1.f / 12.f)
constexpr const float SemitoneRatioLog = 0.08333333333; // std::log2(SemitoneRatio)

float midi_to_freq(int midi);
float freq_to_midi_float(float freq);
int freq_to_midi(float freq);
float freq_to_intonation(float freq);
bool freq_valid(float freq);
bool midi_valid(int midi);
float magnitude_to_velocity(float magnitude);
std::string midi_to_string(int midi);
float wavelength_to_freq(float tau, float sample_rate);
float freq_to_wavelength(float freq, float sample_rate);
int volume_to_velocity(float v);
bool evaluates_to_same_note(float f1, float f2);


/**
 * @brief A pitch estimate
 *
 * This class represents an estimate or guess for a musical pitch
 */
class pitch_estimate {
public:
	pitch_estimate() = default;
	/**
	 * @brief Constructs a pitch_estimate object
	 *
	 * The \p freq parameter denotes the frequency of the pitch estimate
	 */
	explicit pitch_estimate(float freq, float magnitude = 0.f, float confidence = 1.f);
	/**
	 * @brief Constructs a pitch_estimate object
	 *
	 * The \p fft_bin parameter is used to automatically calculate the frequency.
	 */
	explicit pitch_estimate(const bin &fft_bin);
	/**
	 * @brief Checks if this is a valid estimate
	 *
	 * This returns \c false if the pitch is either extremely high or extremely low.
	 * For example the pitch should be playable on a piano.
	 */
	bool valid() const;

	/**
	 * @brief The frequency
	 *
	 * The frequency of the pitch_estimate
	 */
	float frequency = 0.f;
	/**
	 * @brief The magnitude of the pitch_estiamte
	 *
	 * The loudness of the pitch
	 */
	float magnitude = 0.f;
	/**
	 * @brief The confidence of the guess
	 *
	 * This denotes the confidence of the estimate.
	 * 0.0 means not confident, 1.0 means maximum confident.
	 */
	float confidence = 1.f;
	std::string to_csv() const;
};
using pitch_estimates = std::vector<pitch_estimate>;


/**
 * @brief A note estimate
 *
 * This class represents an estimate or guess for a musical note
 */
class note_estimate {
public:
	note_estimate() = default;
	/**
	 * @brief Constructs a note_estimate
	 *
	 * The \p note parameter denotes the Midi note
	 */
	explicit note_estimate(int note, int velocity = 80, float confidence = 1.f);
	/**
	 * @brief Constructs a note_estimate
	 *
	 * Automatically generates the nearest note to the passed pitch_estimate \p p
	 */
	explicit note_estimate(pitch_estimate p);
	/**
	 * @brief Convert to string
	 *
	 * Returns a human-readable representation of the note_estimate
	 */
	std::string to_string() const;
	/**
	 * @brief Check if the musical note is valid
	 *
	 * The note must at least be playable on a standard piano.
	 */
	bool valid() const;

	/**
	 * @brief The musical note
	 *
	 * This is the Midi number of this note_estimate
	 */
	int note = 0;
	/**
	 * @brief The loudness of the note
	 *
	 * This is the Midi velocity of the note
	 */
	int velocity = 80;
	/**
	 * @brief The confidence of the guess
	 *
	 * This denotes the confidence of the estimate.
	 * 0.0 means not confident, 1.0 means maximum confident.
	 */
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
	std::string to_csv() const;
};
std::ostream &operator<<(std::ostream &os, const note_estimate &n);
using note_estimates = std::vector<note_estimate>;

}
