#include "gtest/gtest.h"

#if defined(MAPBAR_WIN32)
#	define chdir _chdir
#endif

int main(int argc, char** argv)
{
	int result;

	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();

	return result;
}
