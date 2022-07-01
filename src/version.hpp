#pragma once

#include <string>

namespace fftune {

#ifndef FFTUNE_VERSION
// fallback version, if cmake failed to set the version
#define FFTUNE_VERSION "1.0"
#endif

std::string version_string();

}
