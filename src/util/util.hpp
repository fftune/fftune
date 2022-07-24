#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

namespace fftune {

std::string to_lower(const std::string &s);
float squared(const float n);
bool is_int(const float n);
void save_string(const std::string &str, const std::filesystem::path &filename);

#ifndef NDEBUG
void debug_csv(const std::string &csv);
#endif


template<typename T>
concept printable = requires(T t) {
	{ std::cout << t } -> std::same_as<std::ostream &>;
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

template<typename T>
std::string vector_to_csv(const std::vector<T> &v) {
	std::string result;
	for (const auto &i : v) {
		result.append(i.to_csv() + "\n");
	}
	return result;
}

template<typename T>
std::string num_vector_to_csv(const std::vector<T> &v) {
	std::string result;
	for (const auto &i : v) {
		result.append(std::to_string(i) + "\n");
	}
	return result;
}

}
