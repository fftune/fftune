#include "util.hpp"

#include <math.h>

namespace fftune {

std::string to_lower(const std::string &s) {
	std::string res;
	res.resize(s.size());
	std::transform(s.begin(), s.end(), res.begin(), [](auto c) { return std::tolower(c); });
	return res;
}

float squared(const float n) {
	return n * n;
}

bool is_int(const float n) {
	float intpart;
	const float fractpart = modf(n, &intpart);
	return fractpart == 0.f;
}

void save_string(const std::string &str, const std::filesystem::path &filename) {
	std::ofstream f {filename};
	f << str;
}

#ifndef NDEBUG
void debug_csv(const std::string &csv) {
	static int csv_num = 0;
	save_string(csv, std::filesystem::temp_directory_path() / (std::to_string(csv_num) + ".csv"));
	++csv_num;
}
#endif

}
