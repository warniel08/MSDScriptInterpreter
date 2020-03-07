//
//  main.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include <iostream>
#include "pointer.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "parse.hpp"
#include "value.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char **argv) {
    try {
        bool optimize_mode = false;
        PTR(Expr) e;
        if ((argc > 1) && !strcmp(argv[1], "--opt")){
            optimize_mode = true;
            argc--;
            argv--;
        }
        if (argc > 1) {
            std::ifstream prog_in(argv[1]);
            e = parse(prog_in);
        } else {
            e = parse(std::cin);
        }
        try {
            if(optimize_mode){
                std::cout << e->optimize()->to_string() << std::endl;
            } else {
                std::cout << e->interp(NEW(EmptyEnv)())->to_string() << std::endl;
            }
        }catch (std::runtime_error err) {
            std::cerr << err.what() << std::endl;
            return 2;
        }
        return 0;
    } catch (std::runtime_error err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
