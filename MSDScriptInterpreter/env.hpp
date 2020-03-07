//
//  env.hpp
//  MSDScriptInterpreter
//
//  Created by Warner Nielsen on 2/28/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#ifndef env_hpp
#define env_hpp

#include <string>
#include "pointer.hpp"

class Val;

class Env {
public:
  virtual PTR(Val) lookup(std::string find_name) = 0;
};

class EmptyEnv : public Env {
public:
  EmptyEnv();
  PTR(Val) lookup(std::string find_name);
};

class ExtendedEnv : public Env {
public:
  std::string name;
  PTR(Val) val;
  PTR(Env) rest;
  
  ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);
  PTR(Val) lookup(std::string find_name);
};


#endif /* env_hpp */
