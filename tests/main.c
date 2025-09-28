#include <picotest.h>

#include "hooks.h"

PICOTEST_SUITE(mainSuite, testUcdPropertyAccessors);

#define PICOTEST_MAINSUITE mainSuite
#include <picotestRunner.inc>
