//
//  expr.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include "expr.hpp"
#include "catch.hpp"

NumExpr::NumExpr(int rep) {
  this->rep = rep;
}

bool NumExpr::equals(Expr *e) {
  NumExpr *n = dynamic_cast<NumExpr*>(e);
  if (n == NULL)
    return false;
  else
    return rep == n->rep;
}

Val *NumExpr::interp() {
  return new NumVal(rep);
}

Expr *NumExpr::subst(std::string var, Val *new_val) {
  return new NumExpr(rep);
}

Expr *NumExpr::optimize() {
  return new NumExpr(rep);
}


bool NumExpr::containsVarExpr() {
  return false;
}

std::string NumExpr::to_string() {
  return std::to_string(rep);
}

AddExpr::AddExpr(Expr *lhs, Expr *rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool AddExpr::equals(Expr *e) {
  AddExpr *a = dynamic_cast<AddExpr*>(e);
  if (a == NULL)
    return false;
  else
    return (lhs->equals(a->lhs) && rhs->equals(a->rhs));
}

Val *AddExpr::interp() {
  return lhs->interp()->add_to(rhs->interp());
}

Expr *AddExpr::subst(std::string var, Val *new_val) {
  return new AddExpr(lhs->subst(var, new_val),
                     (rhs->subst(var, new_val)));
}

Expr *AddExpr::optimize() {
  Expr *olhs = lhs->optimize();
  Expr *orhs = rhs->optimize();
  if (!olhs->containsVarExpr() && !orhs->containsVarExpr())
    return olhs->interp()->add_to(orhs->interp())->to_expr();
  else
    return new AddExpr(olhs, orhs);
}

bool AddExpr::containsVarExpr() {
  return (lhs->containsVarExpr() || rhs->containsVarExpr());
}

std::string AddExpr::to_string() {
  return "(" + lhs->to_string() + " + " + rhs->to_string() + ")";
}

MultExpr::MultExpr(Expr *lhs, Expr *rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool MultExpr::equals(Expr *e) {
  MultExpr *m = dynamic_cast<MultExpr*>(e);
  if (m == NULL)
    return false;
  else
    return (lhs->equals(m->lhs) && rhs->equals(m->rhs));
}

Val *MultExpr::interp() {
  return lhs->interp()->mult_with(rhs->interp());
}

Expr *MultExpr::subst(std::string var, Val *new_val) {
  return new MultExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

Expr *MultExpr::optimize() {
  Expr *olhs = lhs->optimize();
  Expr *orhs = rhs->optimize();
  if (!olhs->containsVarExpr() && !orhs->containsVarExpr())
    return olhs->interp()->mult_with(orhs->interp())->to_expr();
  else
    return new MultExpr(olhs->optimize(), orhs->optimize());
}

bool MultExpr::containsVarExpr() {
  return (lhs->containsVarExpr() || rhs->containsVarExpr());
}

std::string MultExpr::to_string() {
  return "(" + lhs->to_string() + " * " + rhs->to_string() + ")";
}

VarExpr::VarExpr(std::string name) {
  this->name = name;
}

bool VarExpr::equals(Expr *e) {
  VarExpr *v = dynamic_cast<VarExpr*>(e);
  if (v == NULL)
    return false;
  else
    return name.compare(v->name) == 0;
}

Val *VarExpr::interp() {
  throw std::runtime_error("cannot calculate with unknown vars");
}

Expr *VarExpr::subst(std::string var, Val *new_val) {
  if (name == var)
    return new_val->to_expr();
  else
    return new VarExpr(name);
}

Expr *VarExpr::optimize() {
  return new VarExpr(name);
}

bool VarExpr::containsVarExpr() {
  return true;
}

std::string VarExpr::to_string() {
  return name;
}

LetExpr::LetExpr(std::string name, Expr *rhs, Expr *body) {
  this->name = name;
  this->rhs = rhs;
  this->body = body;
}

bool LetExpr::equals(Expr *e) {
  LetExpr *l = dynamic_cast<LetExpr*>(e);
  if (l == NULL)
    return false;
  else
    return (name == l->name && rhs->equals(l->rhs) && body->equals(l->body));
}

Val *LetExpr::interp() {
  return body->subst(name, rhs->interp())->interp();
}

Expr *LetExpr::subst(std::string var, Val *new_val) {
  if (name == var)
    return new LetExpr(name, rhs->subst(var, new_val), body->subst(var, new_val));
  else
    return new LetExpr(name, rhs->subst(var, new_val), body);
}

bool LetExpr::containsVarExpr() {
  return true;
}

Expr *LetExpr::optimize() {
  Expr *orhs = rhs->optimize();
  
  if (orhs->containsVarExpr()) {
    return new LetExpr(name, orhs, body->optimize());
  } else {
    return body->subst(name, orhs->interp())->optimize();
  }
}

std::string LetExpr::to_string() {
  return "(_let " + name + " = " + rhs->to_string() + " _in " + body->to_string() + ")";
}

BoolExpr::BoolExpr(bool rep) {
  this->rep = rep;
}

bool BoolExpr::equals(Expr *e) {
  BoolExpr *b = dynamic_cast<BoolExpr*>(e);
  if (b == NULL)
    return false;
  else
    return rep == b->rep;
}

Val *BoolExpr::interp() {
  return new BoolVal(rep);
}

Expr *BoolExpr::subst(std::string var, Val *new_val) {
  return new BoolExpr(rep);
}

Expr *BoolExpr::optimize() {
  return new BoolExpr(rep);
}


bool BoolExpr::containsVarExpr() {
  return false;
}

std::string BoolExpr::to_string() {
  if (rep)
    return "(_true)";
  else
    return "(_false)";
}

IfExpr::IfExpr(Expr *test_part, Expr *then_part, Expr *else_part) {
  this->test_part = test_part;
  this->then_part = then_part;
  this->else_part = else_part;
}

bool IfExpr::equals(Expr *e) {
  IfExpr *ie = dynamic_cast<IfExpr*>(e);
  if (ie == NULL)
    return false;
  else
    return (test_part->equals(ie->test_part) && then_part->equals(ie->then_part) && else_part->equals(ie->else_part));
}

Val *IfExpr::interp() {
  if (test_part->interp()->is_true())
    return then_part->interp();
  else
    return else_part->interp();
}

Expr *IfExpr::subst(std::string var, Val *new_val) {
  return new IfExpr(test_part->subst(var, new_val), then_part->subst(var, new_val), else_part->subst(var, new_val));
}

Expr *IfExpr::optimize() {
  if (!test_part->containsVarExpr()) {
    if (test_part->interp()->is_true()) {
      return then_part->optimize();
    } else {
      return else_part->optimize();
    }
  }

  return new IfExpr(test_part->optimize(), then_part->optimize(), else_part->optimize());
}

bool IfExpr::containsVarExpr() {
  return (test_part->containsVarExpr() || then_part->containsVarExpr() || else_part->containsVarExpr());
}

std::string IfExpr::to_string() {
  return "(_if " + test_part->to_string() + " _then " + then_part->to_string() + " _else " + else_part->to_string() + ")";
}

CompExpr::CompExpr(Expr *lhs, Expr *rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool CompExpr::equals(Expr *e) {
  CompExpr *ce = dynamic_cast<CompExpr*>(e);
  if (ce == NULL)
    return false;
  else
    return (lhs->equals(ce->lhs) && rhs->equals(ce->rhs));
}

Val *CompExpr::interp() {
  return new BoolVal(lhs->interp()->equals(rhs->interp()));
}

Expr *CompExpr::subst(std::string var, Val *new_val) {
  return new CompExpr(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

Expr *CompExpr::optimize() {
  if (lhs->containsVarExpr() || rhs->containsVarExpr())
    return new CompExpr(lhs->optimize(), rhs->optimize());
  else
    return new BoolExpr(lhs->interp()->equals(rhs->interp()));
}

bool CompExpr::containsVarExpr() {
  return lhs->containsVarExpr() || rhs->containsVarExpr();
}

std::string CompExpr::to_string() {
  return "(" + lhs->to_string() + " == " + rhs->to_string() + ")";
}

FunExpr::FunExpr(std::string formal_arg, Expr *body) {
  this->formal_arg = formal_arg;
  this->body = body;
}

bool FunExpr::equals(Expr *e) {
  FunExpr *fe = dynamic_cast<FunExpr*>(e);
  if (fe == NULL)
    return false;
  else
    return (formal_arg == fe->formal_arg && body->equals(fe->body));
}

Val *FunExpr::interp() {
  return new FunVal(formal_arg, body);
}

Expr *FunExpr::subst(std::string var, Val *new_val) {
  if( var == formal_arg) {
    return new FunExpr(formal_arg, body);
  }
  
  return new FunExpr(formal_arg, body->subst(var, new_val));
}

Expr *FunExpr::optimize() {
  return new FunExpr(formal_arg, body->optimize());
}

bool FunExpr::containsVarExpr() {
  return true;
}

std::string FunExpr::to_string() {
  return "(_fun (" + formal_arg + ") " + body->to_string() + ")";
}

CallExpr::CallExpr(Expr *to_be_called, Expr *actual_arg) {
  this->to_be_called = to_be_called;
  this->actual_arg = actual_arg;
}

bool CallExpr::equals(Expr *e) {
  CallExpr *ce = dynamic_cast<CallExpr*>(e);
  if (ce == NULL)
    return false;
  else
    return (to_be_called->equals(ce->to_be_called) && actual_arg->equals(ce->actual_arg));
}

Val *CallExpr::interp() {
  return to_be_called->interp()->call(actual_arg->interp());
}

Expr *CallExpr::subst(std::string var, Val *new_val) {
  return new CallExpr(to_be_called->subst(var, new_val), actual_arg->subst(var, new_val));
}

Expr *CallExpr::optimize() {
  return new CallExpr(to_be_called->optimize(), actual_arg->optimize());
}

bool CallExpr::containsVarExpr() {
  return true;
}

std::string CallExpr::to_string() {
  return to_be_called->to_string() + " (" + actual_arg->to_string() + ")";
}

TEST_CASE( "NumExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new NumExpr(1))->equals(new NumExpr(1)) );
    CHECK( ! (new NumExpr(1))->equals(new NumExpr(2)) );
    CHECK( ! (new NumExpr(1))->equals(new MultExpr(new NumExpr(2), new NumExpr(4))) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new NumExpr(10))->interp()->equals(new NumVal(10)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new NumExpr(10))->subst("x", new NumVal(4))
          ->equals(new NumExpr(10)) );
  }
  SECTION( "containsVarExpr" ) {
    CHECK( ! (new NumExpr(10))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new NumExpr(4))->to_string()
    == ("4") );
  }
}

TEST_CASE( "VarExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new VarExpr("x"))->equals(new VarExpr("x")) );
    CHECK( ! (new VarExpr("x"))->equals(new NumExpr(5)) );
  }
  
  SECTION( "interp" ) {
    CHECK_THROWS( (new VarExpr("fish"))->interp() );
  }
  
  SECTION( "subst" ) {
    CHECK( (new VarExpr("fish"))->subst("dog", new NumVal(4))
          ->equals(new VarExpr("fish")) );
    CHECK( (new VarExpr("dog"))->subst("dog", new NumVal(4))
          ->equals(new NumExpr(4)) );
    CHECK( (new VarExpr("dog"))->subst("dog", new BoolVal(true))
          ->equals(new BoolExpr(true)) );
    CHECK( (new VarExpr("x"))->subst("x", new NumVal(10))
          ->equals(new NumExpr(10)) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new VarExpr("beef"))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new VarExpr("frog"))->to_string()
          != ("frg") );
    CHECK( (new VarExpr("frog"))->to_string()
          == ("frog") );
  }
}

TEST_CASE( "AddExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new AddExpr(new NumExpr(8), new NumExpr(9))) );
    CHECK( ! (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new AddExpr(new NumExpr(8), new NumExpr(10))) );
    CHECK( ! (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new AddExpr(new NumExpr(10), new NumExpr(9))) );
    CHECK( ! (new AddExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new NumExpr(8)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new AddExpr(new NumExpr(3), new NumExpr(2)))->interp()
          ->equals(new NumVal(5)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new AddExpr(new NumExpr(4), new VarExpr("yak")))->subst("yak", new NumVal(7))
          ->equals(new AddExpr(new NumExpr(4), new NumExpr(7))) );
    CHECK( (new AddExpr(new VarExpr("x"), new NumExpr(4)))->subst("x", new NumVal(3))
          ->equals(new AddExpr(new NumExpr(3), new NumExpr(4))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (new AddExpr(new VarExpr("dog"), new NumExpr(9)))->optimize()
          ->equals(new AddExpr(new VarExpr("dog"), new NumExpr(9))) );
    CHECK( (new AddExpr(new NumExpr(1), new NumExpr(9)))->optimize()
          ->equals(new NumExpr(10)) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new AddExpr(new NumExpr(10), new VarExpr("e")))->containsVarExpr() );
    CHECK( !(new AddExpr(new NumExpr(10), new NumExpr(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new AddExpr(new VarExpr("dog"), new NumExpr(3)))->to_string()
          == ("(dog + 3)") );
    CHECK( (new AddExpr(new NumExpr(5), new NumExpr(3)))->to_string()
          == ("(5 + 3)") );
  }
}

TEST_CASE( "MultExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new MultExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new MultExpr(new NumExpr(8), new NumExpr(9))) );
    CHECK( ! (new MultExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new MultExpr(new NumExpr(8), new NumExpr(10))) );
    CHECK( ! (new MultExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new MultExpr(new NumExpr(10), new NumExpr(9))) );
    CHECK( ! (new MultExpr(new NumExpr(8), new NumExpr(9)))
          ->equals(new NumExpr(8)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new MultExpr(new NumExpr(3), new NumExpr(2)))->interp()
          ->equals(new NumVal(6)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new MultExpr(new NumExpr(2), new VarExpr("dog")))->subst("dog", new NumVal(3))
          ->equals(new MultExpr(new NumExpr(2), new NumExpr(3))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (new MultExpr(new VarExpr("dog"), new NumExpr(9)))->optimize()
          ->equals(new MultExpr(new VarExpr("dog"), new NumExpr(9))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new MultExpr(new NumExpr(10), new VarExpr("e")))->containsVarExpr() );
    CHECK( !(new MultExpr(new NumExpr(10), new NumExpr(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new MultExpr(new VarExpr("dog"), new NumExpr(3)))->to_string()
          == ("(dog * 3)") );
    CHECK( (new MultExpr(new NumExpr(5), new NumExpr(3)))->to_string()
          == ("(5 * 3)") );
  }
}

TEST_CASE( "LetExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new LetExpr("x", new NumExpr(3), new AddExpr(new NumExpr(2), new NumExpr(3))))
          ->equals(new LetExpr("x", new NumExpr(3), new AddExpr(new NumExpr(2), new NumExpr(3)))) );
    CHECK( ! (new LetExpr("x", new NumExpr(3), new AddExpr(new NumExpr(2), new NumExpr(3))))
          ->equals(new VarExpr("y")) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new LetExpr("this", new NumExpr(3), new AddExpr(new NumExpr(2), new NumExpr(3))))->interp()
          ->equals(new NumVal(5)));
    CHECK( (new LetExpr("this", new NumExpr(3), new AddExpr(new VarExpr("this"), new NumExpr(2))))->interp()
          ->equals(new NumVal(5)));
  }
  
  SECTION( "subst" ) {
    CHECK( (new LetExpr("x", new NumExpr(5), new AddExpr(new NumExpr(2), new VarExpr("x"))))
          ->subst(("x"), new NumVal(7))
          ->equals(new LetExpr("x", new NumExpr(5), new AddExpr(new NumExpr(2), new NumExpr(7)))) );
    CHECK( (new LetExpr("x", new NumExpr(5), new AddExpr(new NumExpr(2), new VarExpr("x"))))
          ->subst(("y"), new NumVal(7))
          ->equals(new LetExpr("x", new NumExpr(5), new AddExpr(new NumExpr(2), new VarExpr("x")))) );
    CHECK( (new LetExpr("x", new NumExpr(4), new MultExpr(new NumExpr(2), new VarExpr("x"))))
          ->subst(("x"), new NumVal(11))
          ->equals(new LetExpr("x", new NumExpr(4), new MultExpr(new NumExpr(2), new NumExpr(11)))) );
    CHECK( (new LetExpr("y", new AddExpr(new VarExpr("y"), new NumExpr(1)), new VarExpr("x")))
          ->subst(("y"), new NumVal(7))
          ->equals(new LetExpr("y", new AddExpr(new NumExpr(7), new NumExpr(1)), new VarExpr("x"))) );
    CHECK( (new LetExpr("y", new AddExpr(new VarExpr("y"), new NumExpr(1)), new VarExpr("y")))
          ->subst(("y"), new NumVal(7))
          ->equals(new LetExpr("y", new AddExpr(new NumExpr(7), new NumExpr(1)), new NumExpr(7))) );
    CHECK( (new LetExpr("y", new MultExpr(new VarExpr("y"), new NumExpr(1)), new VarExpr("y")))
          ->subst(("y"), new NumVal(7))
          ->equals(new LetExpr("y", new MultExpr(new NumExpr(7), new NumExpr(1)), new NumExpr(7))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (new LetExpr("x", new NumExpr(3), new AddExpr(new NumExpr(2), new NumExpr(3))))->optimize()
          ->equals(new NumExpr(5)) );
    CHECK( (new LetExpr("x", new NumExpr(3), new MultExpr(new NumExpr(2), new NumExpr(3))))->optimize()
          ->equals(new NumExpr(6)) );
    CHECK( (new LetExpr("x", new VarExpr("y"), new AddExpr(new NumExpr(2), new NumExpr(3))))->optimize()
          ->equals(new LetExpr("x", new VarExpr("y"), new NumExpr(5))));
    CHECK( (new LetExpr("x", new NumExpr(15), new AddExpr(new NumExpr(2), new VarExpr("x"))))->optimize()
          ->equals(new NumExpr(17)) );
    CHECK( (new LetExpr("y", new NumExpr(3), new AddExpr(new NumExpr(2), new VarExpr("x"))))->optimize()
          ->equals(new AddExpr(new NumExpr(2), new VarExpr("x"))) );
    CHECK( (new LetExpr("y", new AddExpr(new NumExpr(5), new VarExpr("y")), new AddExpr(new NumExpr(2), new VarExpr("x"))))->optimize()
          ->equals(new LetExpr("y", new AddExpr(new NumExpr(5), new VarExpr("y")), new AddExpr(new NumExpr(2), new VarExpr("x")))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new LetExpr("x", new NumExpr(10), new NumExpr(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new LetExpr("x", new NumExpr(15), new AddExpr(new NumExpr(2), new VarExpr("x"))))->to_string()
          == ("(_let x = 15 _in (2 + x))") );
    CHECK( (new LetExpr("x", new NumExpr(3), new AddExpr(new VarExpr("x"), new NumExpr(3))))->to_string()
          == ("(_let x = 3 _in (x + 3))"));
    CHECK( (new LetExpr("x", new NumExpr(4), new LetExpr("y", new NumExpr(6), new MultExpr(new VarExpr("x"), new NumExpr(3)))))->to_string()
          == ("(_let x = 4 _in (_let y = 6 _in (x * 3)))"));
  }
}

TEST_CASE( "BoolExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new BoolExpr(true))->equals(new BoolExpr(true)) );
    CHECK( ! (new BoolExpr(false))->equals(new BoolExpr(true)) );
    CHECK( ! (new BoolExpr(true))->equals(new VarExpr("true")) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new BoolExpr(true))->interp()
          ->equals(new BoolVal(true)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new BoolExpr(true))->subst("x", new BoolVal(true))
          ->equals(new BoolExpr(true)));
  }
  
  SECTION( "optimize" ) {
    CHECK( (new BoolExpr(true))->optimize()
          ->equals(new BoolExpr(true)));
    CHECK( (new BoolExpr(false))->optimize()
          ->equals(new BoolExpr(false)));
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new BoolExpr(true))->containsVarExpr()
          == (false));
  }
  
  SECTION( "to_string" ) {
    CHECK( (new BoolExpr(true))->to_string()
          == ("(_true)"));
    CHECK( (new BoolExpr(false))->to_string()
          == ("(_false)"));
  }
}

TEST_CASE( "IfExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new IfExpr(new BoolExpr(true),
            new NumExpr(1),
            new NumExpr(2)))
          ->equals(new IfExpr(new BoolExpr(true),
                              new NumExpr(1),
                              new NumExpr(2))) );
    CHECK( ! (new IfExpr(new BoolExpr(true),
      new NumExpr(1),
      new NumExpr(2)))
          ->equals(new NumExpr(1)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new IfExpr(new BoolExpr(true),
                       new NumExpr(1),
                       new NumExpr(2)))->interp()
          ->equals(new NumVal(1)) );
    CHECK( (new IfExpr(new BoolExpr(false),
                       new AddExpr(new VarExpr("x"), new NumExpr(4)),
                       new NumExpr(2)))->interp()
          ->equals(new NumVal(2)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new IfExpr(new BoolExpr(true),
                       new VarExpr("x"),
                       new NumExpr(2)))
          ->subst("x", new NumVal(3))
          ->equals(new IfExpr(new BoolExpr(true),
                              new NumExpr(3),
                              new NumExpr(2))) );
    CHECK( (new IfExpr(new BoolExpr(true),
                       new AddExpr(new VarExpr("x"), new NumExpr(4)),
                       new MultExpr(new VarExpr("x"), new NumExpr(2))))
          ->subst("x", new NumVal(3))
          ->equals(new IfExpr(new BoolExpr(true),
                              new AddExpr(new NumExpr(3), new NumExpr(4)),
                              new MultExpr(new NumExpr(3), new NumExpr(2)))) );
    CHECK( (new IfExpr(new VarExpr("x"),
                       new VarExpr("x"),
                       new NumExpr(2)))
          ->subst("x", new NumVal(3))
          ->equals(new IfExpr(new NumExpr(3),
                              new NumExpr(3),
                              new NumExpr(2))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (new IfExpr(new BoolExpr(true),new NumExpr(3),new VarExpr("x")))->optimize()
          ->equals(new NumExpr(3)) );
    CHECK( (new IfExpr(new BoolExpr(false),new NumExpr(3),new NumExpr(5)))->optimize()
          ->equals(new NumExpr(5)) );
    CHECK( (new IfExpr(new VarExpr("x"),new NumExpr(3),new VarExpr("y")))->optimize()
          ->equals((new IfExpr(new VarExpr("x"),new NumExpr(3),new VarExpr("y")))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new IfExpr(new BoolExpr(true), new AddExpr(new VarExpr("x"), new NumExpr(4)), new NumExpr(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new IfExpr(new BoolExpr(true), new NumExpr(3), new NumExpr(4)))->to_string()
          == ("(_if (_true) _then 3 _else 4)"));
    CHECK( (new IfExpr(new AddExpr(new VarExpr("x"), new NumExpr(3)), new NumExpr(3), new NumExpr(4)))->to_string()
          == ("(_if (x + 3) _then 3 _else 4)"));
  }
}

TEST_CASE( "CompExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new CompExpr(new NumExpr(1), new NumExpr(1)))
          ->equals(new CompExpr(new NumExpr(1), new NumExpr(1))) );
    CHECK( ! (new CompExpr(new NumExpr(1), new NumExpr(1)))
          ->equals(new NumExpr(1)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new CompExpr(new NumExpr(3), new NumExpr(3)))->interp()
          ->equals(new BoolVal(true)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new CompExpr(new VarExpr("x"), new VarExpr("y")))
          ->subst("x", new NumVal(4))
          ->equals(new CompExpr(new NumExpr(4), new VarExpr("y"))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (new CompExpr(new AddExpr(new NumExpr(3), new NumExpr(4)), new NumExpr(3)))->optimize()
          ->equals(new BoolExpr(false)) );
    CHECK( (new CompExpr(new AddExpr(new VarExpr("x"), new NumExpr(4)), new NumExpr(3)))->optimize()
          ->equals(new CompExpr(new AddExpr(new VarExpr("x"), new NumExpr(4)), new NumExpr(3))) );
    CHECK( (new CompExpr(new VarExpr("x"), new VarExpr("x")))->optimize()
          ->equals(new CompExpr(new VarExpr("x"), new VarExpr("x"))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( ! (new CompExpr(new NumExpr(3), new NumExpr(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new CompExpr(new NumExpr(3), new NumExpr(3)))->to_string()
    == ("(3 == 3)") );
  }
}

TEST_CASE( "FunExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new FunExpr("x", new NumExpr(3)))
          ->equals(new FunExpr("x", new NumExpr(3))) );
    CHECK( ! (new FunExpr("x", new NumExpr(3)))
          ->equals(new NumExpr(3)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new FunExpr("x", new NumExpr(3)))->interp()
          ->equals(new FunVal("x", new NumExpr(3))) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))))->subst("x", new NumVal(3))
    ->equals(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3)))) );
  }

  SECTION( "optimize" ) {
    CHECK( (new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))))->optimize()
          ->equals(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3)))) );
    CHECK( (new FunExpr("x", new AddExpr(new NumExpr(3), new NumExpr(3))))->optimize()
          ->equals(new FunExpr("x", new NumExpr(6))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new FunExpr("x", new NumExpr(3)))->to_string()
          == ("(_fun (x) 3)") );
    CHECK( (new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))))->to_string()
          == ("(_fun (x) (x + 3))") );
  }
}

TEST_CASE( "CallExpr" ) {
  SECTION( "equals" ) {
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3)))
          ->equals(new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3))) );
    CHECK( ! (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3)))
          ->equals(new VarExpr("x")) );
  }
  
  SECTION( "interp" ) {
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3)))->interp()
          ->equals(new NumVal(6)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (new CallExpr(new FunExpr("x", new NumExpr(4)), new NumExpr(3)))->subst("x", new NumVal(2))
          ->equals(new CallExpr(new FunExpr("x", new NumExpr(4)), new NumExpr(3))) );
    CHECK( (new CallExpr(new FunExpr("x", new NumExpr(4)), new NumExpr(3)))->subst("x", new NumVal(2))->to_string()
          == "(_fun (x) 4) (3)" );
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(4)))
          ->subst("x", new NumVal(2))
          ->equals(new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(4))) );
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(4)))
          ->subst("x", new NumVal(2))->to_string()
          == ("(_fun (x) (x + 3)) (4)") );
  }
  
  SECTION( "optimize" ) {
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3)))->optimize()
    ->equals(new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(3))), new NumExpr(5)))->to_string()
          == "(_fun (x) (x + 3)) (5)" );
  }
}
