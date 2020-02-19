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

/* A forward declaration, so `Val` can refer to `Expr`, while
   `Expr` still needs to refer to `Val`. */
class Expr;

class Val {
public:
  virtual bool equals(Val *val) = 0;
  virtual Val *add_to(Val *other_val) = 0;
  virtual Val *mult_with(Val *other_val) = 0;
  virtual Expr *to_expr() = 0;
  virtual std::string to_string() = 0;
  virtual bool is_true() = 0;
  virtual Val *call(Val *actual_arg) = 0;
};

class NumVal : public Val {
public:
  int rep;
  
  NumVal(int rep);
  bool equals(Val *val);

  Val *add_to(Val *other_val);
  Val *mult_with(Val *other_val);
  Expr *to_expr();
  std::string to_string();
  bool is_true();
  Val *call(Val *actual_arg);
};

class BoolVal : public Val {
public:
  bool rep;
  
  BoolVal(bool rep);
  bool equals(Val *val);

  Val *add_to(Val *other_val);
  Val *mult_with(Val *other_val);
  Expr *to_expr();
  std::string to_string();
  bool is_true();
  Val *call(Val *actual_arg);
};

class FunVal : public Val {
public:
  std::string formal_arg;
  Expr *body;
  
  FunVal(std::string formal_arg, Expr *body);
  bool equals(Val *val);

  Val *add_to(Val *other_val);
  Val *mult_with(Val *other_val);
  Expr *to_expr();
  std::string to_string();
  bool is_true();
  Val *call(Val *actual_arg);
};

#endif /* value_hpp */
