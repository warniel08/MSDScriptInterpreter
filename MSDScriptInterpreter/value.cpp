//
//  value.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include <stdexcept>
#include "value.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "catch.hpp"

NumVal::NumVal(int rep) {
  this->rep = rep;
}

bool NumVal::equals(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    return false;
  else
    return rep == other_num_val->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    throw std::runtime_error("not a number");
  else
    return NEW(NumVal)(rep + other_num_val->rep);
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    throw std::runtime_error("not a number");
  else
    return NEW(NumVal)(rep * other_num_val->rep);
}

PTR(Expr)NumVal::to_expr() {
  return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
  return (std::to_string)(rep);
}

bool NumVal::is_true() {
  throw std::runtime_error("can't make numval a bool");
}

PTR(Val) NumVal::call(PTR(Val) actual_arg) {
  throw std::runtime_error("can't use call on numval");
}

BoolVal::BoolVal(bool rep) {
  this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
  PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
  if (other_bool_val == nullptr)
    return false;
  else
    return rep == other_bool_val->rep;
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val) {
  throw std::runtime_error("no adding booleans");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val) {
  throw std::runtime_error("no multiplying booleans");
}

PTR(Expr)BoolVal::to_expr() {
  return NEW(BoolExpr)(rep);
}

std::string BoolVal::to_string() {
  if (rep)
    return "_true";
  else
    return "_false";
}

bool BoolVal::is_true() {
  return rep;
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
  throw std::runtime_error("can't use call on boolval");
}

FunVal::FunVal(std::string formal_arg, PTR(Expr)body, PTR(Env) env) {
  this->formal_arg = formal_arg;
  this->body = body;
  this->env = env;
}

bool FunVal::equals(PTR(Val) other_val) {
  PTR(FunVal) other_fun_val = CAST(FunVal)(other_val);
  if (other_fun_val == nullptr)
    return false;
  else
    return formal_arg == other_fun_val->formal_arg && body->equals(other_fun_val->body);
}

PTR(Val) FunVal::add_to(PTR(Val) other_val) {
  throw std::runtime_error("no adding functions");
}

PTR(Val) FunVal::mult_with(PTR(Val) other_val) {
  throw std::runtime_error("no multiplying functions");
}

PTR(Expr)FunVal::to_expr() {
  return NEW(FunExpr)(formal_arg, body);
}

std::string FunVal::to_string() {
  return "(_fun (" + formal_arg + ") " + body->to_string() + ")";
}

bool FunVal::is_true() {
  throw std::runtime_error("can't make funval a bool");
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
  return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

TEST_CASE( "values equals" ) {
  CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
  CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );

  CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
  CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
  CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );

  CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
  CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );
  
  CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(EmptyEnv)()))
        ->equals(NEW(FunVal)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(EmptyEnv)())) );
  CHECK( ! (NEW(FunVal)("x", NEW(NumExpr)(4), NEW(EmptyEnv)()))
        ->equals(NEW(BoolVal)(false)) );
  CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(EmptyEnv)()))->to_string()
        == "(_fun (x) (3 + 4))" );
}

TEST_CASE( "add_to" ) {
  CHECK ( (NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)) );

  CHECK_THROWS_WITH ( (NEW(NumVal)(5))->add_to(NEW(BoolVal)(false)), "not a number" );
  CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->add_to(NEW(BoolVal)(false)),
                     "no adding booleans" );
  CHECK_THROWS_WITH ( (NEW(FunVal)("x", NEW(NumExpr)(4), NEW(EmptyEnv)()))->add_to(NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)())),
                     "no adding functions" );
}

TEST_CASE( "mult_with" ) {
  
  CHECK ( (NEW(NumVal)(5))->mult_with(NEW(NumVal)(8))->equals(NEW(NumVal)(40)) );

  CHECK_THROWS_WITH ( (NEW(NumVal)(5))->mult_with(NEW(BoolVal)(false)), "not a number" );
  CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->mult_with(NEW(BoolVal)(false)),
                     "no multiplying booleans" );
  CHECK_THROWS_WITH ( (NEW(FunVal)("x", NEW(NumExpr)(4), NEW(EmptyEnv)()))->mult_with(NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)())),
                     "no multiplying functions" );
}

TEST_CASE( "value to_expr" ) {
  CHECK( (NEW(NumVal)(5))->to_expr()->equals(NEW(NumExpr)(5)) );
  CHECK( (NEW(BoolVal)(true))->to_expr()->equals(NEW(BoolExpr)(true)) );
  CHECK( (NEW(BoolVal)(false))->to_expr()->equals(NEW(BoolExpr)(false)) );
  CHECK( (NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)()))->to_expr()->equals(NEW(FunExpr)("x", NEW(NumExpr)(3))) );
}

TEST_CASE( "value to_string" ) {
  CHECK( (NEW(NumVal)(5))->to_string() == "5" );
  CHECK( (NEW(BoolVal)(true))->to_string() == "_true" );
  CHECK( (NEW(BoolVal)(false))->to_string() == "_false" );
  CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)), NEW(EmptyEnv)()))
        ->to_string() == "(_fun (x) (x + 2))" );
  CHECK( (NEW(FunVal)("n", NEW(FunExpr)("m", NEW(AddExpr)(NEW(VarExpr)("n"), NEW(VarExpr)("m"))), NEW(EmptyEnv)()))
        ->to_string() == "(_fun (n) (_fun (m) (n + m)))" );
}

TEST_CASE( "value is_true" ) {
  CHECK_THROWS_WITH( (NEW(NumVal)(3))->is_true(), "can't make numval a bool" );
  CHECK( (NEW(BoolVal)(true))->is_true() );
  CHECK( ! (NEW(BoolVal)(false))->is_true() );
  CHECK_THROWS_WITH( (NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)()))->is_true(), "can't make funval a bool" );
}

TEST_CASE( "value call" ) {
  CHECK_THROWS_WITH( (NEW(NumVal)(4))->call(NEW(NumVal)(2)), "can't use call on numval" );
  CHECK_THROWS_WITH( (NEW(BoolVal)(true))->call(NEW(BoolVal)(false)), "can't use call on boolval" );
  CHECK( (NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)()))->call(NEW(NumVal)(4))
        ->equals(NEW(NumVal)(3)) );
  CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), NEW(EmptyEnv)()))->call(NEW(NumVal)(4))
        ->equals(NEW(NumVal)(7)) );
  CHECK( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),NEW(EmptyEnv)()))->call(NEW(NumVal)(4))
        ->equals(NEW(NumVal)(12)) );
}
