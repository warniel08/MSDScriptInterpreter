//
//  value.hpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

  
#ifndef value_hpp
#define value_hpp

#include <iostream>
#include "pointer.hpp"

/* A forward declaration, so `Val` can refer to `Expr`, while
   `Expr` still needs to refer to `Val`. */
class Expr;
class Env;

class Val {
public:
  virtual bool equals(PTR(Val) val) = 0;
  virtual PTR(Val) add_to(PTR(Val) other_val) = 0;
  virtual PTR(Val) mult_with(PTR(Val) other_val) = 0;
  virtual PTR(Expr) to_expr() = 0;
  virtual std::string to_string() = 0;
  virtual bool is_true() = 0;
  virtual PTR(Val) call(PTR(Val) actual_arg) = 0;
};

class NumVal : public Val {
public:
  int rep;
  
  NumVal(int rep);
  bool equals(PTR(Val) val);

  PTR(Val) add_to(PTR(Val) other_val);
  PTR(Val) mult_with(PTR(Val) other_val);
  PTR(Expr) to_expr();
  std::string to_string();
  bool is_true();
  PTR(Val) call(PTR(Val) actual_arg);
};

class BoolVal : public Val {
public:
  bool rep;
  
  BoolVal(bool rep);
  bool equals(PTR(Val) val);

  PTR(Val) add_to(PTR(Val) other_val);
  PTR(Val) mult_with(PTR(Val) other_val);
  PTR(Expr) to_expr();
  std::string to_string();
  bool is_true();
  PTR(Val) call(PTR(Val) actual_arg);
};

class FunVal : public Val {
public:
  std::string formal_arg;
  PTR(Expr) body;
  PTR(Env) env;
  
  FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
  bool equals(PTR(Val) val);

  PTR(Val) add_to(PTR(Val) other_val);
  PTR(Val) mult_with(PTR(Val) other_val);
  PTR(Expr) to_expr();
  std::string to_string();
  bool is_true();
  PTR(Val) call(PTR(Val) actual_arg);
};

#endif /* value_hpp */
