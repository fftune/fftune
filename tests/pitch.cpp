#include "pitch/pitch.hpp"
#include "tests.hpp"

class PitchTest : public ::testing::Test {
protected:
	const float error = 0.001f;
	void SetUp() override {
	}
};


TEST_F(PitchTest, Translation) {
	// tests that pitch estimates are translated correctly to note estimates
	float freq = fftune::FreqA4;
	// go through a whole octave
	for (int midi = fftune::MidiA4; midi < fftune::MidiA4 + 12; ++midi, freq *= fftune::SemitoneRatio) {
		auto pitch = fftune::pitch_estimate(freq);
		// translate
		auto note = fftune::note_estimate(pitch);

		// expect the correct note
		EXPECT_EQ(note.note, midi);
		// expect good intonation since the note's pitch is right on
		EXPECT_NEAR(note.intonation, 0.f, error);
	}
}

TEST_F(PitchTest, Volume) {
	// check that volume is translated correctly, the higher note is initialized louder than the lower one
	auto high_pitch = fftune::pitch_estimate(fftune::FreqA4, -10.f);
	auto low_pitch = fftune::pitch_estimate(fftune::FreqA4 / 2.f, -30.f);

	// translate
	auto high_note = fftune::note_estimate(high_pitch);
	auto low_note = fftune::note_estimate(low_pitch);

	EXPECT_EQ(high_note.note, fftune::MidiA4);
	EXPECT_EQ(low_note.note, fftune::MidiA4 - 12);
	EXPECT_GT(high_note.velocity, low_note.velocity);
}

TEST_F(PitchTest, Intonation) {
	// test that intonation is translated correctly

	// 33ct sharp/flat
	const float factor = 1.f / 3.f;
	const float drift_off = std::pow(fftune::SemitoneRatio, factor);

	// perfect intonation
	auto pitch = fftune::pitch_estimate(fftune::FreqA4);
	auto note = fftune::note_estimate(pitch);
	EXPECT_FLOAT_EQ(note.intonation, 0.f);

	// very sharp
	pitch = fftune::pitch_estimate(fftune::FreqA4 * drift_off);
	note = fftune::note_estimate(pitch);
	EXPECT_NEAR(note.intonation, factor, error);

	// very flat
	pitch = fftune::pitch_estimate(fftune::FreqA4 / drift_off);
	note = fftune::note_estimate(pitch);
	EXPECT_NEAR(note.intonation, -factor, error);
}
