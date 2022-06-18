#pragma once

#include "fft/fft.hpp"

namespace fftune {

class double_fft {
public:
	explicit double_fft(const config &conf);
	note_estimates detect(const sample_buffer &in);
private:
	config conf;
	fft spec;
};

}
