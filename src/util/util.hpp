#pragma once

#include <ranges>
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

namespace fftune {

std::string to_lower(const std::string &s);
float squared(const float n);
bool is_int(const float n);
void save_string(const std::string &str, const std::filesystem::path &filename);


template<typename T>
concept printable = requires(T t) {
	{ std::cout << t } -> std::same_as<std::ostream&>;
};

template<printable t>
inline std::ostream &operator<<(std::ostream &os, const std::vector<t> &c) {
	os << "{";
	if (!c.empty()) {
		os << c.front();
	}
	for (size_t i = 1; i < c.size(); ++i) {
		os << ",\t" << c[i];
	}
	os << "}";
	return os;
}

template<printable t>
void verbose_log(const t &loggable, const bool verbose) {
	if (verbose) {
		std::cout << loggable << std::endl;
	}
}

}
