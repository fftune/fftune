#include "tests.hpp"

class FftTest : public :: testing::Test {
protected:
	fftune::sample_buffer buf {tests::config.buffer_size};
};


void check_bins(const fftune::bins &bins, float error_margin = 2.f, float expected_peakfreq = fftune::FreqA4) {
	float best_freq = 0.f;
	float best_value = std::numeric_limits<float>::lowest();
	float closest_fdistance = std::numeric_limits<float>::max();
	for (const auto &i : bins) {
		if (i.magnitude > best_value) {
			best_freq = i.frequency;
			best_value = i.magnitude;
		}
		closest_fdistance = std::min(closest_fdistance, std::abs(i.frequency - expected_peakfreq));
	}

	// the highest peak must be the closest to the true frequency or at least within a close margin
	EXPECT_LT(std::abs(best_freq - expected_peakfreq), closest_fdistance + error_margin);
}

TEST_F(FftTest, Peak) {
	auto fft = fftune::fft(tests::config.buffer_size, tests::config.sample_rate);
	fftune::gen_sine(fftune::FreqA4, tests::config.sample_rate, buf.data, buf.size);

	// check that the highest peak is indeed the correct frequency
	const auto bins = fft.detect(buf);
	check_bins(bins);
}

TEST_F(FftTest, Sizes) {
	// fft should work for all sorts of different buffer sizes
	constexpr const size_t max_bufsize = 32768;
	size_t buf_size = tests::config.buffer_size;
	for (; buf_size <= max_bufsize; buf_size *= 2) {
		fftune::sample_buffer b {buf_size};
		// init is actually the bottleneck here, so optimize for it
		auto fft = fftune::fft (b.size, tests::config.sample_rate, fftune::fft_heuristic::OptimizeInit);

		fftune::gen_sine(fftune::FreqA4, tests::config.sample_rate, b.data, b.size);
		const auto bins = fft.detect(b);
		// check that we have the expected amount of bins
		EXPECT_EQ(bins.size(), fft.bins_size());

		check_bins(bins);
	}
}

TEST_F(FftTest, Samplerates) {
	// fft should work for all sorts of different sample rates
	// list of common sample rates
	constexpr const std::array sample_rates = {8000.f, 16000.f, 44100.f, 48000.f, 96000.f, 192000.f};
	for (const auto sample_rate : sample_rates) {
		fftune::fft fft {buf.size, sample_rate};
		fftune::gen_sine(fftune::FreqA4, sample_rate, buf.data, buf.size);
		const auto bins = fft.detect(buf);

		/**
		 * Larger frequency domain for the same buffer size means that fft bins are spaced more widely apart
		 * Therefore allow for more error the larger the sample rate
		 */
		float error_margin = 2.f;
		if (sample_rate > 48000.f) {
			error_margin += sample_rate / buf.size;
		}
		check_bins(bins, error_margin);
	}
}

TEST_F(FftTest, Repeat) {
	/**
	 * There is a very weird bug in fftw3 where if we don't init the input buffer, memory gets corrupted
	 * And successive FFT calls have different results
	 * To check for this phenomenon, we repeat multiple FFTs and compare the results just to be sure that we never ever hit this ugly bug again
	 */
	fftune::bins last_bins;
	for (size_t i = 0; i < 100; ++i) {
		auto fft = fftune::fft(tests::config.buffer_size, tests::config.sample_rate);
		fftune::gen_sine(fftune::FreqA4, tests::config.sample_rate, buf.data, buf.size);
		const auto bins = fft.detect(buf);
		check_bins(bins);

		// check that the return values are absolutely identical
		if (!last_bins.empty()) {
			EXPECT_EQ(last_bins, bins);
		}
		last_bins = bins;
		EXPECT_FALSE(last_bins.empty());
	}
}

TEST_F(FftTest, Heuristics) {
	// should work with different fftw heuristics
	constexpr const std::array heuristics = {fftune::fft_heuristic::OptimizeInit, fftune::fft_heuristic::OptimizeRuntime};
	for (auto h : heuristics) {
		auto fft = fftune::fft(tests::config.buffer_size, tests::config.sample_rate, h);
		fftune::gen_sine(fftune::FreqA4, tests::config.sample_rate, buf.data, buf.size);
		const auto bins = fft.detect(buf);
		check_bins(bins);
	}
}
