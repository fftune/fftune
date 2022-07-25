#include "tests.hpp"

class SamplebufferTest : public ::testing::Test {
protected:
	fftune::sample_buffer buf {tests::config.buffer_size};
	void SetUp() override {
		for (size_t i = 0; i < buf.size; ++i) {
			buf.data[i] = i;
		}
	}
};


TEST_F(SamplebufferTest, Storage) {
	EXPECT_EQ(buf.size, tests::config.buffer_size);
	// test that the buffer holds the correct data
	for (size_t i = 0; i < buf.size; ++i) {
		EXPECT_FLOAT_EQ(buf.data[i], i);
	}
}

TEST_F(SamplebufferTest, WriteBack) {
	float buffer[tests::config.buffer_size];
	// initialize buffer with invalid data
	for (size_t i = 0; i < buf.size; ++i) {
		buffer[i] = -1.f;
	}

	buf.write(buffer);
	// test that the data has been written correctly
	for (size_t i = 0; i < buf.size; ++i) {
		EXPECT_FLOAT_EQ(buffer[i], i);
		EXPECT_FLOAT_EQ(buffer[i], buf.data[i]);
	}
}

TEST_F(SamplebufferTest, Cycle) {
	constexpr const auto hop = tests::config.buffer_size / 4;
	// cycle buffer
	buf.cycle(hop);

	// now the values must be shifted by that amount
	for (size_t i = 0; i < buf.size - hop; ++i) {
		EXPECT_FLOAT_EQ(buf.data[i], i + hop);
	}
}

TEST_F(SamplebufferTest, ZeroCycle) {
	buf.cycle(0);
	// rotating by 0 should not have done anything
	for (size_t i = 0; i < buf.size; ++i) {
		EXPECT_FLOAT_EQ(buf.data[i], i);
	}
}
