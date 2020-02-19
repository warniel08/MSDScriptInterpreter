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
#include "value.hpp"

/*
 * Objects to be returned by the parser
 * */
class Expr{
public:
  virtual bool equals(Expr *e) = 0;
  
  // To compute the number value of an expression,
  // assuming that all variables are 0
  virtual Val *interp() = 0;
  
  // To substitute a number in place of a variable
  virtual Expr *subst(std::string var, Val *val) = 0;
  
  virtual Expr *optimize() = 0;
  
  // return true or false if Expr * has a variable
  virtual bool containsVarExpr() = 0;
  
  virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
  int rep;
  
  NumExpr(int rep);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class AddExpr : public Expr {
public:
  Expr *lhs;
  Expr *rhs;
  
  AddExpr(Expr *lhs, Expr *rhs);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class MultExpr : public Expr {
public:
  Expr *lhs;
  Expr *rhs;
  
  MultExpr(Expr *lhs, Expr *rhs);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class VarExpr : public Expr {
public:
  std::string name;
  
  VarExpr(std::string name);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class LetExpr : public Expr {
public:
  std::string name;
  Expr *rhs;
  Expr *body;
  
  LetExpr(std::string name, Expr *rhs, Expr *body);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class BoolExpr : public Expr {
public:
  bool rep;
  
  BoolExpr(bool rep);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class IfExpr : public Expr {
public:
  Expr *test_part;
  Expr *then_part;
  Expr *else_part;
  
  IfExpr(Expr *test_part, Expr *then_part, Expr *else_part);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class CompExpr : public Expr {
public:
  Expr *lhs;
  Expr *rhs;
  
  CompExpr(Expr *lhs, Expr *rhs);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class FunExpr : public Expr {
public:
  std::string formal_arg;
  Expr *body;
  
  FunExpr(std::string formal_arg, Expr *body);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

class CallExpr : public Expr {
public:
  Expr *to_be_called;
  Expr *actual_arg;
  
  CallExpr(Expr *lhs, Expr *rhs);
  bool equals(Expr *e);
  
  Val *interp();
  Expr *subst(std::string var, Val *val);
  Expr *optimize();
  
  bool containsVarExpr();
  std::string to_string();
};

#endif /* expr_hpp */
