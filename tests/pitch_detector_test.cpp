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


TEST_F(PitchDetectorTest, yin) {
	fftune::pitch_detector<fftune::yin_config> p {tests::config};
	const auto notes = p.detect(buf);
	ASSERT_FALSE(notes.empty());
	// we expect A4
	ASSERT_EQ(fftune::MidiA4, notes.front().note);
}
