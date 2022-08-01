#include "bin.hpp"

namespace fftune {

bin::bin(float real, float img, float magnitude, float freq) {
	value = {real, img};
	this->magnitude = magnitude;
	this->frequency = freq;
}

bin::bin(float real, float img, size_t index, size_t num_samples, float sample_rate) {
	value = {real, img};
	this->magnitude = 20.f * log10(2.f * this->norm() / num_samples);
	this->frequency = sample_rate / num_samples * index;
}

float bin::norm() const {
	return std::abs(value);
}

float bin::phase() const {
	return std::arg(value);
}

float bin::real() const {
	return value.real();
}

float bin::img() const {
	return value.imag();
}

bool operator==(const bin &l, const bin &r) {
	return l.real() == r.real() && l.img() == r.img() && l.frequency == r.frequency;
}

std::string bins_to_csv(const bins &b) {
	/**
	 * For example these can be plotted in gnuplot with:
	 * set datafile separator ","
	 * plot '/path/to/csv' using 1:2
	 */
	std::string result;
	for (const auto &i : b) {
		result.append(std::to_string(i.frequency) + "," + std::to_string(i.magnitude) + "\n");
	}
	return result;
}

bins discrete_derivative(const bins &b) {
	bins result;
	result.reserve(b.size());

	/**
	 * The first value can't be compared to anything
	 * Usually one would make the derivative of size n - 1
	 * But in this case it is more elegant, if the derivative has the same size
	 * So instead we just declare the first element as zero
	 */
	result.push_back(bin(0.f, 0.f, 0.f, 0.f));

	float last_value = 0.f;
	for (size_t i = 1; i < b.size(); ++i) {
		const auto diff = b[i].magnitude - last_value;
		last_value = b[i].magnitude;
		result.push_back(bin(b[i].real(), b[i].img(), diff, b[i].frequency));
	}

	return result;
}

float bins_distance(const bins &a, const bins &b) {
	float result = 0.f;
	// lower value means more similar
	constexpr int neighbors = 10;
	for (int i = 0; i < a.size(); ++i) {
		float mean_a = 0.f;
		float mean_b = 0.f;
		size_t count = 0;
		for (int j = std::min(0, i - neighbors); j < std::max(i + 10, static_cast<int>(a.size())); ++j) {
			mean_a += a[j].magnitude;
			mean_b += b[j].magnitude;
			++count;
		}
		mean_a /= count;
		mean_b /= count;

		float standard_deviation_a = 0.f;
		float standard_deviation_b = 0.f;
		for (int j = std::min(0, i - neighbors); j < std::max(i + 10, static_cast<int>(a.size())); ++j) {
			float diff = a[j].magnitude - mean_a;
			standard_deviation_a += diff * diff;

			diff = b[j].magnitude - mean_b;
			standard_deviation_b += diff * diff;
		}
		standard_deviation_a = std::sqrt(standard_deviation_a / (count - 1));
		standard_deviation_b = std::sqrt(standard_deviation_b / (count - 1));

		float dev_a = (a[i].magnitude - mean_a) / standard_deviation_a;
		float dev_b = (b[i].magnitude - mean_b) / standard_deviation_b;

		auto delta = dev_a - dev_b;
		result += delta * delta * delta * delta;
	}
	return result;
}

void bins_normalize_pos(bins &b) {
	// force only positive values by shifting everything up such that the minimum is now zero
	const auto min_magnitude = std::ranges::min_element(b, [](const auto &l, const auto &r) { return l.magnitude < r.magnitude; })->magnitude;
	std::ranges::for_each(b, [&](auto &p) { p.magnitude += min_magnitude; });
}

void bins_normalize_sin(bins &b) {
	const int local_width = b.size() / 300;
	std::vector<float> magnitudes;
	magnitudes.resize(b.size());
	for (int i = 0; i < b.size(); ++i) {
		// compute local mean
		float local_mean = 0.f;
		size_t num_locals = 0;
		for (int j = std::max(i - local_width, 0); j < std::min(i + local_width, static_cast<int>(b.size())); ++j) {
			local_mean += b[j].magnitude;
			++num_locals;
		}
		local_mean /= num_locals;

		// compute deviation
		float max_deviation = 0.f;
		for (int j = std::max(i - local_width, 0); j < std::min(i + local_width, static_cast<int>(b.size())); ++j) {
			max_deviation = std::max(max_deviation, std::abs(b[j].magnitude - local_mean));
		}
		magnitudes[i] = (b[i].magnitude - local_mean) / max_deviation;
	}

	// copy magnitudes
	for (size_t i = 0; i < b.size(); ++i) {
		b[i].magnitude = magnitudes[i];
	}
}

}
