
#define CATCH_CONFIG_NO_POSIX_SIGNALS
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "../mrudp/base/Core.h"

using namespace timprepscius;
using namespace core;

int main( int argc, char* argv[] )
{
	xLogInitialize("mrudp-test.log");
	
	//xLogActivateStory(LogAllStories);
//	xLogActivateStory("mrudp::send");
//	xLogActivateStory("mrudp::receive");
//	xLogActivateStory("mrudp::retry");
	
	using namespace Catch::clara;

	Catch::Session session;
	auto cli = session.cli();
	session.cli(cli);
	
	session.applyCommandLine(argc, argv);

	int result = session.run( argc, argv );
	
	return result;
}
