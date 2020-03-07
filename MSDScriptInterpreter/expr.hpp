//
//  expr.hpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#ifndef expr_hpp
#define expr_hpp

#include <string>
#include "pointer.hpp"

/*
 * Objects to be returned by the parser
 * */
class Val;
class Env;

class Expr {
public:
  virtual bool equals(PTR(Expr) e) = 0;
  
  // To compute the number value of an expression,
  // assuming that all variables are 0
  virtual PTR(Val) interp(PTR(Env) env) = 0;
  
  // To substitute a number in place of a variable
  virtual PTR(Expr) subst(std::string var, PTR(Val) val) = 0;
  
  virtual PTR(Expr) optimize() = 0;
  
  // return true or false if PTR(Expr)  has a variable
  virtual bool containsVarExpr() = 0;
  
  virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
  int rep;
  PTR(Val) val;
  
  NumExpr(int rep);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class AddExpr : public Expr {
public:
  PTR(Expr) lhs;
  PTR(Expr) rhs;
  
  AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class MultExpr : public Expr {
public:
  PTR(Expr) lhs;
  PTR(Expr) rhs;
  
  MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class VarExpr : public Expr {
public:
  std::string name;
  
  VarExpr(std::string name);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class LetExpr : public Expr {
public:
  std::string name;
  PTR(Expr) rhs;
  PTR(Expr) body;
  
  LetExpr(std::string name, PTR(Expr) rhs, PTR(Expr) body);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class BoolExpr : public Expr {
public:
  bool rep;
  
  BoolExpr(bool rep);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class IfExpr : public Expr {
public:
  PTR(Expr) test_part;
  PTR(Expr) then_part;
  PTR(Expr) else_part;
  
  IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class CompExpr : public Expr {
public:
  PTR(Expr) lhs;
  PTR(Expr) rhs;
  
  CompExpr(PTR(Expr) lhs, PTR(Expr) rhs);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class FunExpr : public Expr {
public:
  std::string formal_arg;
  PTR(Expr) body;
  
  FunExpr(std::string formal_arg, PTR(Expr) body);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class CallExpr : public Expr {
public:
  PTR(Expr) to_be_called;
  PTR(Expr) actual_arg;
  
  CallExpr(PTR(Expr) lhs, PTR(Expr) rhs);
  bool equals(PTR(Expr) e);
  
  PTR(Val) interp(PTR(Env) env);
  PTR(Expr) subst(std::string var, PTR(Val) val);
  PTR(Expr) optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

#endif /* expr_hpp */
