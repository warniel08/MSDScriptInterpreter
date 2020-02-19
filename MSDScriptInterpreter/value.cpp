//
//  value.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include "expr.hpp"
#include <stdexcept>
#include "catch.hpp"

NumVal::NumVal(int rep) {
  this->rep = rep;
}

bool NumVal::equals(Val *other_val) {
  NumVal *other_num_val = dynamic_cast<NumVal*>(other_val);
  if (other_num_val == nullptr)
    return false;
  else
    return rep == other_num_val->rep;
}

Val *NumVal::add_to(Val *other_val) {
  NumVal *other_num_val = dynamic_cast<NumVal*>(other_val);
  if (other_num_val == nullptr)
    throw std::runtime_error("not a number");
  else
    return new NumVal(rep + other_num_val->rep);
}

Val *NumVal::mult_with(Val *other_val) {
  NumVal *other_num_val = dynamic_cast<NumVal*>(other_val);
  if (other_num_val == nullptr)
    throw std::runtime_error("not a number");
  else
    return new NumVal(rep * other_num_val->rep);
}

Expr *NumVal::to_expr() {
  return new NumExpr(rep);
}

std::string NumVal::to_string() {
  return (std::to_string)(rep);
}

bool NumVal::is_true() {
  throw std::runtime_error("can't make numval a bool");
}

Val *NumVal::call(Val *actual_arg) {
  throw std::runtime_error("can't use call on numval");
}

BoolVal::BoolVal(bool rep) {
  this->rep = rep;
}

bool BoolVal::equals(Val *other_val) {
  BoolVal *other_bool_val = dynamic_cast<BoolVal*>(other_val);
  if (other_bool_val == nullptr)
    return false;
  else
    return rep == other_bool_val->rep;
}

Val *BoolVal::add_to(Val *other_val) {
  throw std::runtime_error("no adding booleans");
}

Val *BoolVal::mult_with(Val *other_val) {
  throw std::runtime_error("no multiplying booleans");
}

Expr *BoolVal::to_expr() {
  return new BoolExpr(rep);
}

std::string BoolVal::to_string() {
  if (rep)
    return "(_true)";
  else
    return "(_false)";
}

bool BoolVal::is_true() {
  return rep;
}

Val *BoolVal::call(Val *actual_arg) {
  throw std::runtime_error("can't use call on boolval");
}

FunVal::FunVal(std::string formal_arg, Expr *body) {
  this->formal_arg = formal_arg;
  this->body = body;
}

bool FunVal::equals(Val *other_val) {
  FunVal *other_fun_val = dynamic_cast<FunVal*>(other_val);
  if (other_fun_val == nullptr)
    return false;
  else
    return formal_arg == other_fun_val->formal_arg && body->equals(other_fun_val->body);
}

Val *FunVal::add_to(Val *other_val) {
  throw std::runtime_error("no adding functions");
}

Val *FunVal::mult_with(Val *other_val) {
  throw std::runtime_error("no multiplying functions");
}

Expr *FunVal::to_expr() {
  return new FunExpr(formal_arg, body);
}

std::string FunVal::to_string() {
  return "(_fun (" + formal_arg + ")) " + body->to_string();
}

bool FunVal::is_true() {
  throw std::runtime_error("can't make funval a bool");
}

Val *FunVal::call(Val *actual_arg) {
  return body->subst(formal_arg, actual_arg)->interp();
}

TEST_CASE( "values equals" ) {
  CHECK( (new NumVal(5))->equals(new NumVal(5)) );
  CHECK( ! (new NumVal(7))->equals(new NumVal(5)) );

  CHECK( (new BoolVal(true))->equals(new BoolVal(true)) );
  CHECK( ! (new BoolVal(true))->equals(new BoolVal(false)) );
  CHECK( ! (new BoolVal(false))->equals(new BoolVal(true)) );

  CHECK( ! (new NumVal(7))->equals(new BoolVal(false)) );
  CHECK( ! (new BoolVal(false))->equals(new NumVal(8)) );
  
  CHECK( (new FunVal("x", new AddExpr(new NumExpr(3), new NumExpr(4))))
        ->equals(new FunVal("x", new AddExpr(new NumExpr(3), new NumExpr(4)))) );
  CHECK( ! (new FunVal("x", new NumExpr(4)))
        ->equals(new BoolVal(false)) );
  CHECK( (new FunVal("x", new AddExpr(new NumExpr(3), new NumExpr(4))))->to_string()
        == "(_fun (x)) (3 + 4)" );
}

TEST_CASE( "add_to" ) {
  
  CHECK ( (new NumVal(5))->add_to(new NumVal(8))->equals(new NumVal(13)) );

  CHECK_THROWS_WITH ( (new NumVal(5))->add_to(new BoolVal(false)), "not a number" );
  CHECK_THROWS_WITH ( (new BoolVal(false))->add_to(new BoolVal(false)),
                     "no adding booleans" );
  CHECK_THROWS_WITH ( (new FunVal("x", new NumExpr(4)))->add_to(new FunVal("x", new NumExpr(3))),
                     "no adding functions" );
}

TEST_CASE( "mult_with" ) {
  
  CHECK ( (new NumVal(5))->mult_with(new NumVal(8))->equals(new NumVal(40)) );

  CHECK_THROWS_WITH ( (new NumVal(5))->mult_with(new BoolVal(false)), "not a number" );
  CHECK_THROWS_WITH ( (new BoolVal(false))->mult_with(new BoolVal(false)),
                     "no multiplying booleans" );
  CHECK_THROWS_WITH ( (new FunVal("x", new NumExpr(4)))->mult_with(new FunVal("x", new NumExpr(3))),
                     "no multiplying functions" );
}

TEST_CASE( "value to_expr" ) {
  CHECK( (new NumVal(5))->to_expr()->equals(new NumExpr(5)) );
  CHECK( (new BoolVal(true))->to_expr()->equals(new BoolExpr(true)) );
  CHECK( (new BoolVal(false))->to_expr()->equals(new BoolExpr(false)) );
  CHECK( (new FunVal("x", new NumExpr(3)))->to_expr()->equals(new FunExpr("x", new NumExpr(3))) );
}

TEST_CASE( "value to_string" ) {
  CHECK( (new NumVal(5))->to_string() == "5" );
  CHECK( (new BoolVal(true))->to_string() == "(_true)" );
  CHECK( (new BoolVal(false))->to_string() == "(_false)" );
  CHECK( (new FunVal("x", new AddExpr(new VarExpr("x"), new NumExpr(2))))
        ->to_string() == "(_fun (x)) (x + 2)" );
  CHECK( (new FunVal("n", new FunExpr("m", new AddExpr(new VarExpr("n"), new VarExpr("m")))))
        ->to_string() == "(_fun (n)) (_fun (m) (n + m))" );
}

TEST_CASE( "value is_true" ) {
  CHECK_THROWS_WITH( (new NumVal(3))->is_true(), "can't make numval a bool" );
  CHECK( (new BoolVal(true))->is_true() );
  CHECK( ! (new BoolVal(false))->is_true() );
  CHECK_THROWS_WITH( (new FunVal("x", new NumExpr(3)))->is_true(), "can't make funval a bool" );
}

TEST_CASE( "value call" ) {
  CHECK_THROWS_WITH( (new NumVal(4))->call(new NumVal(2)), "can't use call on numval" );
  CHECK_THROWS_WITH( (new BoolVal(true))->call(new BoolVal(false)), "can't use call on boolval" );
  CHECK( (new FunVal("x", new NumExpr(3)))->call(new NumVal(4))
        ->equals(new NumVal(3)) );
  CHECK( (new FunVal("x", new AddExpr(new VarExpr("x"), new NumExpr(3))))->call(new NumVal(4))
        ->equals(new NumVal(7)) );
  CHECK( (new FunVal("x", new MultExpr(new VarExpr("x"), new NumExpr(3))))->call(new NumVal(4))
        ->equals(new NumVal(12)) );
}
