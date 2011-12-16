#include <stdio.h>

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_LONGLONG long long
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

extern void board_start(const char*);

int main( void ) {
    CxxTest::ErrorPrinter tmp;

    board_start(__FILE__);

    CxxTest::Main<CxxTest::ErrorPrinter>( tmp, 0, NULL );

    printf("Tests complete.  Stopping\r\n");
	while(1)
	{
	}
}
#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
