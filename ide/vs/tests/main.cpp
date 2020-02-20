#define CATCH_CONFIG_RUNNER
#include <../test/catch.h>
#include <cstdio>

using namespace std;

int main(int argc, char** argv)
{
	Catch::Session().run(argc, argv);
	getchar();
	return 0;
}