//
//  main.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include <iostream>
#include "parse.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char** argv) {
  try {
    Expr *e = parse(std::cin);
    if ((argc > 1) && !strcmp(argv[1], "--optimize")){
      std::cout << e->optimize()->to_string() << std::endl;
    }else
      std::cout << e->interp()->to_string() << std::endl;
    return 0;
  } catch (std::runtime_error exn) {
    std::cerr << exn.what() << std::endl;
    return 1;
  }
}
