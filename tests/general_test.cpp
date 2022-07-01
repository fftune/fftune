#include "tests.hpp"

TEST(General, Version) {
	// version should be set
	EXPECT_FALSE(fftune::version_string().empty());
}
