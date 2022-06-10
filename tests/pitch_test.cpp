#include "tests.hpp"

class PitchTest : public ::testing::Test {
protected:
	fftune::sample_buffer buf {tests::config.buffer_size};
	void SetUp() override {
		fftune::gen_harmonic(fftune::FreqA4, tests::config.sample_rate, buf.data, buf.size);
	}
};


TEST_F(PitchTest, yin) {
	fftune::pitch_detector<fftune::yin_config> p {tests::config};
	const auto notes = p.detect(buf);
	ASSERT_FALSE(notes.empty());
	// we expect A4
	ASSERT_EQ(fftune::MidiA4, notes.front().note);
}
