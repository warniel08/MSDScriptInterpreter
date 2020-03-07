//
//  expr.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include "expr.hpp"
#include "catch.hpp"
#include "value.hpp"
#include "env.hpp"

NumExpr::NumExpr(int rep) {
  this->rep = rep;
  val = NEW(NumVal)(rep);
}

bool NumExpr::equals(PTR(Expr) e) {
  PTR(NumExpr) n = CAST(NumExpr)(e);
  if (n == NULL)
    return false;
  else
    return rep == n->rep;
}

PTR(Val) NumExpr::interp(PTR(Env) env) {
  return val;
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(NumExpr)(rep);
}

PTR(Expr) NumExpr::optimize() {
  return NEW(NumExpr)(rep);
}


bool NumExpr::containsVarExpr() {
  return false;
}

std::string NumExpr::to_string() {
  return std::to_string(rep);
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e) {
  PTR(AddExpr) a = CAST(AddExpr)(e);
  if (a == NULL)
    return false;
  else
    return (lhs->equals(a->lhs) && rhs->equals(a->rhs));
}

PTR(Val) AddExpr::interp(PTR(Env) env) {
  return lhs->interp(env)->add_to(rhs->interp(env));
}

PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(AddExpr)(lhs->subst(var, new_val),
                     (rhs->subst(var, new_val)));
}

PTR(Expr) AddExpr::optimize() {
  PTR(Expr) olhs = lhs->optimize();
  PTR(Expr) orhs = rhs->optimize();
  if (!olhs->containsVarExpr() && !orhs->containsVarExpr())
    return olhs->interp(NEW(EmptyEnv)())->add_to(orhs->interp(NEW(EmptyEnv)()))->to_expr();
  else
    return NEW(AddExpr)(olhs, orhs);
}

bool AddExpr::containsVarExpr() {
  return (lhs->containsVarExpr() || rhs->containsVarExpr());
}

std::string AddExpr::to_string() {
  return "(" + lhs->to_string() + " + " + rhs->to_string() + ")";
}

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e) {
  PTR(MultExpr) m = CAST(MultExpr)(e);
  if (m == NULL)
    return false;
  else
    return (lhs->equals(m->lhs) && rhs->equals(m->rhs));
}

PTR(Val) MultExpr::interp(PTR(Env) env) {
  return lhs->interp(env)->mult_with(rhs->interp(env));
}

PTR(Expr) MultExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(MultExpr)(lhs->subst(var, new_val),
                      rhs->subst(var, new_val));
}

PTR(Expr) MultExpr::optimize() {
  PTR(Expr) olhs = lhs->optimize();
  PTR(Expr) orhs = rhs->optimize();
  if (!olhs->containsVarExpr() && !orhs->containsVarExpr())
    return olhs->interp(NEW(EmptyEnv)())->mult_with(orhs->interp(NEW(EmptyEnv)()))->to_expr();
  else
    return NEW(MultExpr)(olhs->optimize(), orhs->optimize());
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

bool VarExpr::equals(PTR(Expr) e) {
  PTR(VarExpr) v = CAST(VarExpr)(e);
  if (v == NULL)
    return false;
  else
    return name.compare(v->name) == 0;
}

PTR(Val) VarExpr::interp(PTR(Env) env) {
  return env->lookup(name);
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_val) {
  if (name == var)
    return new_val->to_expr();
  else
    return NEW(VarExpr)(name);
}

PTR(Expr) VarExpr::optimize() {
  return NEW(VarExpr)(name);
}

bool VarExpr::containsVarExpr() {
  return true;
}

std::string VarExpr::to_string() {
  return name;
}

LetExpr::LetExpr(std::string name, PTR(Expr) rhs, PTR(Expr) body) {
  this->name = name;
  this->rhs = rhs;
  this->body = body;
}

bool LetExpr::equals(PTR(Expr) e) {
  PTR(LetExpr) l = CAST(LetExpr)(e);
  if (l == NULL)
    return false;
  else
    return (name == l->name && rhs->equals(l->rhs) && body->equals(l->body));
}

PTR(Val) LetExpr::interp(PTR(Env) env) {
  PTR(Val) rhs_val = rhs->interp(env);
  PTR(Env) new_env = NEW(ExtendedEnv) (name, rhs_val, env);
  return body->interp(new_env);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) new_val) {
  if (name == var)
    return NEW(LetExpr)(name, rhs->subst(var, new_val), body->subst(var, new_val));
  else
    return NEW(LetExpr)(name, rhs->subst(var, new_val), body);
}

bool LetExpr::containsVarExpr() {
  return true;
}

PTR(Expr) LetExpr::optimize() {
  PTR(Expr) orhs = rhs->optimize();
  
  if (orhs->containsVarExpr()) {
    return NEW(LetExpr)(name, orhs, body->optimize());
  } else {
    return body->subst(name, orhs->interp(NEW(EmptyEnv)()))->optimize();
  }
}

std::string LetExpr::to_string() {
  return "(_let " + name + " = " + rhs->to_string() + " _in " + body->to_string() + ")";
}

BoolExpr::BoolExpr(bool rep) {
  this->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
  PTR(BoolExpr) b = CAST(BoolExpr)(e);
  if (b == NULL)
    return false;
  else
    return rep == b->rep;
}

PTR(Val) BoolExpr::interp(PTR(Env) env) {
  return NEW(BoolVal)(rep);
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(BoolExpr)(rep);
}

PTR(Expr) BoolExpr::optimize() {
  return NEW(BoolExpr)(rep);
}


bool BoolExpr::containsVarExpr() {
  return false;
}

std::string BoolExpr::to_string() {
  if (rep)
    return "_true";
  else
    return "_false";
}

IfExpr::IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part) {
  this->test_part = test_part;
  this->then_part = then_part;
  this->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e) {
  PTR(IfExpr) ie = CAST(IfExpr)(e);
  if (ie == NULL)
    return false;
  else
    return (test_part->equals(ie->test_part) && then_part->equals(ie->then_part) && else_part->equals(ie->else_part));
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
  if (test_part->interp(env)->is_true())
    return then_part->interp(env);
  else
    return else_part->interp(env);
}

PTR(Expr) IfExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(IfExpr)(test_part->subst(var, new_val), then_part->subst(var, new_val), else_part->subst(var, new_val));
}

PTR(Expr) IfExpr::optimize() {
  if (!test_part->containsVarExpr()) {
    if (test_part->interp(NEW(EmptyEnv)())->is_true()) {
      return then_part->optimize();
    } else {
      return else_part->optimize();
    }
  }

  return NEW(IfExpr)(test_part->optimize(), then_part->optimize(), else_part->optimize());
}

bool IfExpr::containsVarExpr() {
  return (test_part->containsVarExpr() || then_part->containsVarExpr() || else_part->containsVarExpr());
}

std::string IfExpr::to_string() {
  return "(_if " + test_part->to_string() + " _then " + then_part->to_string() + " _else " + else_part->to_string() + ")";
}

CompExpr::CompExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool CompExpr::equals(PTR(Expr) e) {
  PTR(CompExpr) ce = CAST(CompExpr)(e);
  if (ce == NULL)
    return false;
  else
    return (lhs->equals(ce->lhs) && rhs->equals(ce->rhs));
}

PTR(Val) CompExpr::interp(PTR(Env) env) {
  return NEW(BoolVal)(lhs->interp(env)->equals(rhs->interp(env)));
}

PTR(Expr) CompExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(CompExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

PTR(Expr) CompExpr::optimize() {
  if (lhs->containsVarExpr() || rhs->containsVarExpr())
    return NEW(CompExpr)(lhs->optimize(), rhs->optimize());
  else
    return NEW(BoolExpr)(lhs->interp(NEW(EmptyEnv)())->equals(rhs->interp(NEW(EmptyEnv)())));
}

bool CompExpr::containsVarExpr() {
  return lhs->containsVarExpr() || rhs->containsVarExpr();
}

std::string CompExpr::to_string() {
  return "(" + lhs->to_string() + " == " + rhs->to_string() + ")";
}

FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body) {
  this->formal_arg = formal_arg;
  this->body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
  PTR(FunExpr) fe = CAST(FunExpr)(e);
  if (fe == NULL)
    return false;
  else
    return (formal_arg == fe->formal_arg && body->equals(fe->body));
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
  return NEW(FunVal)(formal_arg, body, env);
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) new_val) {
  if( var == formal_arg) {
    return NEW(FunExpr)(formal_arg, body);
  }
  
  return NEW(FunExpr)(formal_arg, body->subst(var, new_val));
}

PTR(Expr) FunExpr::optimize() {
  return NEW(FunExpr)(formal_arg, body->optimize());
}

bool FunExpr::containsVarExpr() {
  return true;
}

std::string FunExpr::to_string() {
  return "(_fun (" + formal_arg + ") " + body->to_string() + ")";
}

CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
  this->to_be_called = to_be_called;
  this->actual_arg = actual_arg;
}

bool CallExpr::equals(PTR(Expr) e) {
  PTR(CallExpr) ce = CAST(CallExpr)(e);
  if (ce == NULL)
    return false;
  else
    return (to_be_called->equals(ce->to_be_called) && actual_arg->equals(ce->actual_arg));
}

PTR(Val) CallExpr::interp(PTR(Env) env) {
  return to_be_called->interp(env)->call(actual_arg->interp(env));
}

PTR(Expr) CallExpr::subst(std::string var, PTR(Val) new_val) {
  return NEW(CallExpr)(to_be_called->subst(var, new_val), actual_arg->subst(var, new_val));
}

PTR(Expr) CallExpr::optimize() {
  return NEW(CallExpr)(to_be_called->optimize(), actual_arg->optimize());
}

bool CallExpr::containsVarExpr() {
  return true;
}

std::string CallExpr::to_string() {
  return to_be_called->to_string() + " (" + actual_arg->to_string() + ")";
}

TEST_CASE( "NumExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(NumExpr)(10))->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(10)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(NumExpr)(10))->subst("x", NEW(NumVal)(4))
          ->equals(NEW(NumExpr)(10)) );
  }
  SECTION( "containsVarExpr" ) {
    CHECK( ! (NEW(NumExpr)(10))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(NumExpr)(4))->to_string()
    == ("4") );
  }
}

TEST_CASE( "VarExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(VarExpr)("x"))->equals(NEW(VarExpr)("x")) );
    CHECK( ! (NEW(VarExpr)("x"))->equals(NEW(NumExpr)(5)) );
  }
  
  SECTION( "interp" ) {
    CHECK_THROWS( (NEW(VarExpr)("fish"))->interp(NEW(EmptyEnv)()) );
    CHECK( (NEW(VarExpr)("fish"))->interp(NEW(ExtendedEnv)("fish",
                                                          NEW(NumVal)(12), NEW(EmptyEnv)()))
                                              ->equals(NEW(NumVal)(12)) );
    
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(VarExpr)("fish"))->subst("dog", NEW(NumVal)(4))
          ->equals(NEW(VarExpr)("fish")) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(NumVal)(4))
          ->equals(NEW(NumExpr)(4)) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(BoolVal)(true))
          ->equals(NEW(BoolExpr)(true)) );
    CHECK( (NEW(VarExpr)("x"))->subst("x", NEW(NumVal)(10))
          ->equals(NEW(NumExpr)(10)) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(VarExpr)("beef"))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(VarExpr)("frog"))->to_string()
          != ("frg") );
    CHECK( (NEW(VarExpr)("frog"))->to_string()
          == ("frog") );
  }
}

TEST_CASE( "AddExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(NumExpr)(8)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(5)) );
    CHECK( (NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y")))
            ->interp(NEW(ExtendedEnv)("x", NEW(NumVal)(4),
                                              NEW(ExtendedEnv)("y", NEW(NumVal)(3),
                                                  NEW(EmptyEnv)())))
          ->equals(NEW(NumVal)(7)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(4), NEW(VarExpr)("yak")))->subst("yak", NEW(NumVal)(7))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(7))) );
    CHECK( (NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))->subst("x", NEW(NumVal)(3))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(AddExpr)(NEW(VarExpr)("dog"), NEW(NumExpr)(9)))->optimize()
          ->equals(NEW(AddExpr)(NEW(VarExpr)("dog"), NEW(NumExpr)(9))) );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(9)))->optimize()
          ->equals(NEW(NumExpr)(10)) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(10), NEW(VarExpr)("e")))->containsVarExpr() );
    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(AddExpr)(NEW(VarExpr)("dog"), NEW(NumExpr)(3)))->to_string()
          == ("(dog + 3)") );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(3)))->to_string()
          == ("(5 + 3)") );
  }
}

TEST_CASE( "MultExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(10), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
          ->equals(NEW(NumExpr)(8)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(6)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(MultExpr)(NEW(VarExpr)("dog"), NEW(NumExpr)(9)))->optimize()
          ->equals(NEW(MultExpr)(NEW(VarExpr)("dog"), NEW(NumExpr)(9))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(10), NEW(VarExpr)("e")))->containsVarExpr() );
    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(10), NEW(NumExpr)(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(MultExpr)(NEW(VarExpr)("dog"), NEW(NumExpr)(3)))->to_string()
          == ("(dog * 3)") );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(3)))->to_string()
          == ("(5 * 3)") );
  }
}

TEST_CASE( "LetExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( ! (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))))
          ->equals(NEW(VarExpr)("y")) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(LetExpr)("this", NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(5)) );
    CHECK( (NEW(LetExpr)("this", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("this"), NEW(NumExpr)(2))))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(5)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))
          ->subst(("x"), NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(7)))) );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))
          ->subst(("y"), NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))) );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(4), NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))
          ->subst(("x"), NEW(NumVal)(11))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(4), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(11)))) );
    CHECK( (NEW(LetExpr)("y", NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(1)), NEW(VarExpr)("x")))
          ->subst(("y"), NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("y", NEW(AddExpr)(NEW(NumExpr)(7), NEW(NumExpr)(1)), NEW(VarExpr)("x"))) );
    CHECK( (NEW(LetExpr)("y", NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(1)), NEW(VarExpr)("y")))
          ->subst(("y"), NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("y", NEW(AddExpr)(NEW(NumExpr)(7), NEW(NumExpr)(1)), NEW(NumExpr)(7))) );
    CHECK( (NEW(LetExpr)("y", NEW(MultExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(1)), NEW(VarExpr)("y")))
          ->subst(("y"), NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("y", NEW(MultExpr)(NEW(NumExpr)(7), NEW(NumExpr)(1)), NEW(NumExpr)(7))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))))->optimize()
          ->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))))->optimize()
          ->equals(NEW(NumExpr)(6)) );
    CHECK( (NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))))->optimize()
          ->equals(NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(NumExpr)(5))) );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(15), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))->optimize()
          ->equals(NEW(NumExpr)(17)) );
    CHECK( (NEW(LetExpr)("y", NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))->optimize()
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))) );
    CHECK( (NEW(LetExpr)("y", NEW(AddExpr)(NEW(NumExpr)(5), NEW(VarExpr)("y")), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))->optimize()
          ->equals(NEW(LetExpr)("y", NEW(AddExpr)(NEW(NumExpr)(5), NEW(VarExpr)("y")), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(10), NEW(NumExpr)(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(15), NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))))->to_string()
          == ("(_let x = 15 _in (2 + x))") );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->to_string()
          == ("(_let x = 3 _in (x + 3))"));
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(4), NEW(LetExpr)("y", NEW(NumExpr)(6), NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))))->to_string()
          == ("(_let x = 4 _in (_let y = 6 _in (x * 3)))"));
  }
}

TEST_CASE( "BoolExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(true)) );
    CHECK( ! (NEW(BoolExpr)(false))->equals(NEW(BoolExpr)(true)) );
    CHECK( ! (NEW(BoolExpr)(true))->equals(NEW(VarExpr)("true")) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(BoolExpr)(true))->interp(NEW(EmptyEnv)())
          ->equals(NEW(BoolVal)(true)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(BoolExpr)(true))->subst("x", NEW(BoolVal)(true))
          ->equals(NEW(BoolExpr)(true)) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(BoolExpr)(true))->optimize()
          ->equals(NEW(BoolExpr)(true)) );
    CHECK( (NEW(BoolExpr)(false))->optimize()
          ->equals(NEW(BoolExpr)(false)) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(BoolExpr)(true))->containsVarExpr()
          == (false) );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(BoolExpr)(true))->to_string()
          == ("_true") );
    CHECK( (NEW(BoolExpr)(false))->to_string()
          == ("_false") );
  }
}

TEST_CASE( "IfExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true),
            NEW(NumExpr)(1),
            NEW(NumExpr)(2)))
          ->equals(NEW(IfExpr)(NEW(BoolExpr)(true),
                              NEW(NumExpr)(1),
                              NEW(NumExpr)(2))) );
    CHECK( ! (NEW(IfExpr)(NEW(BoolExpr)(true),
                         NEW(NumExpr)(1),
                         NEW(NumExpr)(2)))
          ->equals(NEW(NumExpr)(1)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true),
                       NEW(NumExpr)(1),
                       NEW(NumExpr)(2)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(1)) );
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(false),
                       NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
                       NEW(NumExpr)(2)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(2)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true),
                       NEW(VarExpr)("x"),
                       NEW(NumExpr)(2)))
          ->subst("x", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(BoolExpr)(true),
                              NEW(NumExpr)(3),
                              NEW(NumExpr)(2))) );
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true),
                       NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
                       NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))))
          ->subst("x", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(BoolExpr)(true),
                              NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)),
                              NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))) );
    CHECK( (NEW(IfExpr)(NEW(VarExpr)("x"),
                       NEW(VarExpr)("x"),
                       NEW(NumExpr)(2)))
          ->subst("x", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(NumExpr)(3),
                              NEW(NumExpr)(3),
                              NEW(NumExpr)(2))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true),NEW(NumExpr)(3),NEW(VarExpr)("x")))->optimize()
          ->equals(NEW(NumExpr)(3)) );
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(false),NEW(NumExpr)(3),NEW(NumExpr)(5)))->optimize()
          ->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(IfExpr)(NEW(VarExpr)("x"),NEW(NumExpr)(3),NEW(VarExpr)("y")))->optimize()
          ->equals((NEW(IfExpr)(NEW(VarExpr)("x"),NEW(NumExpr)(3),NEW(VarExpr)("y")))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)),
                       NEW(NumExpr)(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(3), NEW(NumExpr)(4)))->to_string()
          == ("(_if _true _then 3 _else 4)"));
    CHECK( (NEW(IfExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), NEW(NumExpr)(3), NEW(NumExpr)(4)))->to_string()
          == ("(_if (x + 3) _then 3 _else 4)"));
  }
}

TEST_CASE( "CompExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(CompExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1)))
          ->equals(NEW(CompExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1))) );
    CHECK( ! (NEW(CompExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1)))
          ->equals(NEW(NumExpr)(1)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(BoolVal)(true)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y")))
          ->subst("x", NEW(NumVal)(4))
          ->equals(NEW(CompExpr)(NEW(NumExpr)(4), NEW(VarExpr)("y"))) );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(3)))->optimize()
          ->equals(NEW(BoolExpr)(false)) );
    CHECK( (NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)), NEW(NumExpr)(3)))->optimize()
          ->equals(NEW(CompExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)), NEW(NumExpr)(3))) );
    CHECK( (NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))->optimize()
          ->equals(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( ! (NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(CompExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)))->to_string()
    == ("(3 == 3)") );
  }
}

TEST_CASE( "FunExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(FunExpr)("x", NEW(NumExpr)(3)))
          ->equals(NEW(FunExpr)("x", NEW(NumExpr)(3))) );
    CHECK( ! (NEW(FunExpr)("x", NEW(NumExpr)(3)))
          ->equals(NEW(NumExpr)(3)) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(FunExpr)("x", NEW(NumExpr)(3)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)())) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->subst("x", NEW(NumVal)(3))
    ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))) );
  }

  SECTION( "optimize" ) {
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))) );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(NumExpr)(6))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(FunExpr)("x", NEW(NumExpr)(3)))->interp(NEW(EmptyEnv)())->to_string()
          == ("(_fun (x) 3)") );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->to_string()
          == ("(_fun (x) (x + 3))") );
  }
}

TEST_CASE( "CallExpr" ) {
  SECTION( "equals" ) {
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3)))
          ->equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3))) );
    CHECK( ! (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3)))
          ->equals(NEW(VarExpr)("x")) );
  }
  
  SECTION( "interp" ) {
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3)))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(6)) );
  }
  
  SECTION( "subst" ) {
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(NumExpr)(4)), NEW(NumExpr)(3)))->subst("x", NEW(NumVal)(2))
          ->equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(NumExpr)(4)), NEW(NumExpr)(3))) );
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(NumExpr)(4)), NEW(NumExpr)(3)))->subst("x", NEW(NumVal)(2))->to_string()
          == "(_fun (x) 4) (3)" );
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(4)))
          ->subst("x", NEW(NumVal)(2))
          ->equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(4))) );
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(4)))
          ->subst("x", NEW(NumVal)(2))->to_string()
          == ("(_fun (x) (x + 3)) (4)") );
  }
  
  SECTION( "optimize" ) {
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3)))->optimize()
    ->equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3))) );
  }
  
  SECTION( "containsVarExpr" ) {
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(3)))->containsVarExpr() );
  }
  
  SECTION( "to_string" ) {
    CHECK( (NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(5)))->to_string()
          == "(_fun (x) (x + 3)) (5)" );
  }
  
  SECTION( "test macros conversion" ) {
    PTR(Expr) add_e = NEW(AddExpr) (NEW(VarExpr)("x"), NEW(NumExpr)(5));
    PTR(Val) three_v = NEW(NumVal)(3);
    CHECK( add_e->subst("x", three_v)->to_string() == "(3 + 5)");
  }
}
