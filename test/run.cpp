//
//  run.cpp
//  test
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include <stdio.h>

extern "C" {
#include "run.h"
}

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

bool run_tests() {
  const char *argv[] = { "HW03ArithParser_updated" };
  return (Catch::Session().run(1, argv) == 0);
}
