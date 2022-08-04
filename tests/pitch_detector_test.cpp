#include "tests.hpp"

class PitchDetectorTest : public ::testing::Test {
protected:
	fftune::sample_buffer buf {tests::config.buffer_size};
	fftune::tone_generator gen;
	void SetUp() override {
		gen.init(tests::config.buffer_size, tests::config.sample_rate, "");
		gen.gen_harmonics(buf, {fftune::note_estimate(fftune::MidiA4)});
	}
};


TEST_F(PitchDetectorTest, Schmitt) {
	fftune::pitch_detector<fftune::schmitt_config> p {tests::config};
	const auto notes = p.detect(buf);
	ASSERT_FALSE(notes.empty());
	// we expect A4
	ASSERT_EQ(fftune::MidiA4, notes.front().note);
}

TEST_F(PitchDetectorTest, Yin) {
	fftune::pitch_detector<fftune::yin_config> p {tests::config};
	const auto notes = p.detect(buf);
	ASSERT_FALSE(notes.empty());
	// we expect A4
	ASSERT_EQ(fftune::MidiA4, notes.front().note);
}

TEST_F(PitchDetectorTest, Comb) {
	fftune::pitch_detector<fftune::fast_comb_config> p {tests::config};
	const auto notes = p.detect(buf);
	ASSERT_FALSE(notes.empty());
	// we expect A4
	ASSERT_EQ(fftune::MidiA4, notes.front().note);
}

TEST_F(PitchDetectorTest, Sfizz) {
	fftune::pitch_detector<fftune::fftune_sfizz_config> p {tests::config};
	const auto notes = p.detect(buf);
	ASSERT_FALSE(notes.empty());
	// we expect A4
	ASSERT_EQ(fftune::MidiA4, notes.front().note);
}

TEST_F(PitchDetectorTest, Polyphonic) {
	fftune::config conf = tests::config;
	// test polyphony
	conf.max_polyphony = 2;
	fftune::pitch_detector<fftune::fftune_sfizz_config> p {conf};

	// generate D4 and A4 simultaneously
	const int d4 = fftune::MidiA4 - 7;
	gen.gen_harmonics(buf, {fftune::note_estimate(d4), fftune::note_estimate(fftune::MidiA4)});

	const auto notes = p.detect(buf);
	// two notes should be detected
	ASSERT_EQ(2, notes.size());

	/**
	 * the order of the notes does not matter
	 * we just need to check that one of them is D4 and the other one is A4
	 */
	ASSERT_TRUE((notes[0].note == fftune::MidiA4 && notes[1].note == d4) || notes[0].note == d4 && notes[1].note == fftune::MidiA4);
}
