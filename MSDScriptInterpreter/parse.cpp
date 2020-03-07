//
//  parse.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include "parse.hpp"
#include "catch.hpp"
#include "expr.hpp"
#include "value.hpp"
#include "env.hpp"

#include <iostream>
#include <sstream>

/*
 * Functions that return the expression objects
 * */
static PTR(Expr) parse_expr(std::istream &in);
static PTR(Expr) parse_comparg(std::istream &in);
static PTR(Expr) parse_addend(std::istream &in);
static PTR(Expr) parse_multicand(std::istream &in);
static PTR(Expr) parse_inner(std::istream &in);
static PTR(Expr) parse_number(std::istream &in);
static PTR(Expr) parse_variable(std::istream &in);
static std::string parse_keyword(std::istream &in);
static std::string parse_alphabetic(std::istream &in, std::string prefix);
static char peek_after_spaces(std::istream &in);
static PTR(Expr) parse_let(std::istream &in);
static PTR(Expr) parse_if(std::istream &in);
static PTR(Expr) parse_fun(std::istream &in);

// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse errors.
PTR(Expr) parse(std::istream &in) {
  PTR(Expr) e = parse_expr(in);
  
  char c = peek_after_spaces(in);
  if (!in.eof())
    throw std::runtime_error((std::string)"expected end of file at " + c);
  
  return e;
}

// Takes an input stream that starts with an expression,
// consuming the largest initial expression possible.
static PTR(Expr) parse_expr(std::istream &in) {
  PTR(Expr) e = parse_comparg(in);
  
  char c = peek_after_spaces(in);
  if (c == '=') {
    c = in.get();
    c = in.get();
    if (c == '=') {
      PTR(Expr) rhs = parse_expr(in);
      e = NEW(CompExpr)(e, rhs);
    } else
       throw std::runtime_error("not a comp expr");
  }
  
  return e;
}

static PTR(Expr) parse_comparg(std::istream &in) {
  PTR(Expr) e = parse_addend(in);
  
  char c = peek_after_spaces(in);
  if (c == '+') {
    in >> c;
    PTR(Expr) rhs = parse_comparg(in);
    e = NEW(AddExpr)(e, rhs);
  }
  
  return e;
}

// Takes an input stream that starts with an addend,
// consuming the largest initial addend possible, where
// an addend is an expression that does not have `+`
// except within nested expressions (like parentheses).
static PTR(Expr) parse_addend(std::istream &in) {
  PTR(Expr) e = parse_multicand(in);
  
  char c = peek_after_spaces(in);
  if (c == '*') {
    c = in.get();
    PTR(Expr) rhs = parse_addend(in);
    e = NEW(MultExpr)(e, rhs);
  }
  
  return e;
}

static PTR(Expr) parse_multicand(std::istream &in) {
  PTR(Expr) e = parse_inner(in);
  
  while (peek_after_spaces(in) == '(') {
    in.get();
    PTR(Expr) actual_arg = parse_expr(in);
    e = NEW(CallExpr)(e, actual_arg);
    if (peek_after_spaces(in) == ')')
      in.get();
    else
      throw std::runtime_error("expected a ) paren");
  }
  
  return e;
}

// Parses something with no immediate `+` or `*` from `in`.
static PTR(Expr) parse_inner(std::istream &in) {
  PTR(Expr) e;
  
  char c = peek_after_spaces(in);
  
  if (c == '(') {
    c = in.get();
    e = parse_expr(in);
    c = peek_after_spaces(in);
    if (c == ')')
      c = in.get();
    else
      throw std::runtime_error("expected a close parenthesis");
  } else if (c == '-') {
    e = parse_number(in);
  } else if (isdigit(c)) {
    e = parse_number(in);
  } else if (isalpha(c)) {
    e = parse_variable(in);
  }
  else if (c == '_') {
    std::string keyword = parse_keyword(in);
    if (keyword == "_let")
      e = parse_let(in);
    else if (keyword == "_false")
      return NEW(BoolExpr)(false);
    else if (keyword == "_true")
      return NEW(BoolExpr)(true);
    else if (keyword == "_if")
      e = parse_if(in);
    else if (keyword == "_fun")
      e = parse_fun(in);
    else
      throw std::runtime_error((std::string)"unexpected keyword " + keyword);
  } else {
    throw std::runtime_error((std::string)"expected a digit or open parenthesis or letter at " + c);
  }
  
  return e;
}

// Parses a number, assuming that `in` starts with a digit.
static PTR(Expr) parse_number(std::istream &in) {
  char c = peek_after_spaces(in);
  if (c == '-')
    c = in.get();
  int num = 0;
  in >> num;
  if (c == '-')
    num *= -1;
  return NEW(NumExpr)(num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
static PTR(Expr) parse_variable(std::istream &in) {
  return NEW(VarExpr)(parse_alphabetic(in, ""));
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_keyword(std::istream &in) {
  in.get(); // consume `_`
  return parse_alphabetic(in, "_");
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_alphabetic(std::istream &in, std::string prefix) {
  std::string name = prefix;
  while (1) {
    char c = in.peek();
    if (!isalpha(c))
      break;
    name += in.get();
  }
  return name;
}

// _let variable = Expr _in Expr
static PTR(Expr) parse_let(std::istream &in) {
  char c;
  c = peek_after_spaces(in);
  std::string retParseAlphaName = parse_alphabetic(in, "");
  
  c = peek_after_spaces(in);
  in.get();
  
  c = peek_after_spaces(in);
  PTR(Expr) rhs = parse_expr(in);
  
  c = peek_after_spaces(in);
  std::string pKeyWord = parse_keyword(in);
  
  c = peek_after_spaces(in);
  PTR(Expr) body = parse_expr(in);
  
  PTR(Expr) newLet = NEW(LetExpr)(retParseAlphaName, rhs, body);
  
  return newLet;
}

static PTR(Expr) parse_if(std::istream &in) {
  PTR(Expr) test_part = parse_expr(in);
  std::string then_clause = parse_keyword(in);
  
  if (then_clause != "_then")
    throw std::runtime_error("expected a _then keyword");
  
  PTR(Expr) then_part = parse_expr(in);
  std::string else_clause = parse_keyword(in);
  
  if (else_clause != "_else")
    throw std::runtime_error("expected an _else keyword");
  
  PTR(Expr) else_part = parse_expr(in);
  
  return NEW(IfExpr)(test_part, then_part, else_part);
}

static PTR(Expr) parse_fun(std::istream &in) {
  char c = peek_after_spaces(in);
  
  if (c != '(')
    throw std::runtime_error("expected a ( paren");
  
  in.get();
  std::string formal_arg = parse_alphabetic(in, "");
  c = peek_after_spaces(in);
  
  if (c != ')')
    throw std::runtime_error("expected a ) paren");
  in.get();
  c = peek_after_spaces(in);
  
  PTR(Expr) body = parse_expr(in);
  
  return NEW(FunExpr)(formal_arg, body);
}

// Like in.peek(), but consume an whitespace at the
// start of `in`
static char peek_after_spaces(std::istream &in) {
  char c;
  while (1) {
    c = in.peek();
    if (!isspace(c))
      break;
    c = in.get();
  }
  return c;
}

/* for tests */
static PTR(Expr) parse_str(std::string s) {
  std::istringstream in(s);
  return parse(in);
}

/* for tests */
static std::string parse_str_error(std::string s) {
  std::istringstream in(s);
  try {
    (void)parse(in);
    return "no exception";
  } catch (std::runtime_error exn) {
    return exn.what();
  }
}

TEST_CASE( "Object tests" ) {
  PTR(Expr) ten_plus_one = NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1));
  PTR(Expr) minus_ten_plus_one = NEW(AddExpr)(NEW(NumExpr)(-10), NEW(NumExpr)(1));
  
  CHECK ( parse_str_error(" (1 ") == "expected a close parenthesis" );
  CHECK ( parse_str_error(" ( 1 ") == "expected a close parenthesis" );
  CHECK ( parse_str_error(" _ ") == "unexpected keyword _");
  CHECK ( parse_str_error(" ! ") == "expected a digit or open parenthesis or letter at !" );
  CHECK ( parse_str_error(" (1 ") == "expected a close parenthesis" );
  CHECK ( parse_str_error(" ! ") == "expected a digit or open parenthesis or letter at !" );
  CHECK ( parse_str_error(" 1 )") == "expected end of file at )" );
  CHECK ( parse_str_error(" 7 = 5 ") == "not a comp expr" );
  CHECK ( parse_str_error(" (_fun 5) ") == "expected a ( paren" );
  CHECK ( parse_str_error(" _fun (x ") == "expected a ) paren" );
  CHECK ( parse_str_error(" _fun (x) (x + 1) (2 ") == "expected a ) paren" );
  CHECK ( parse_str_error(" _if _true ") == "expected a _then keyword" );
  CHECK ( parse_str_error(" _if _true _then 8 ") == "expected an _else keyword" );
  
  CHECK( parse_str("10")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str("-10")->equals(NEW(NumExpr)(-10)) );
  CHECK( parse_str("(10)")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str("(-10)")->equals(NEW(NumExpr)(-10)) );
  CHECK( parse_str("10+1")->equals(ten_plus_one) );
  CHECK( parse_str("-10+1")->equals(minus_ten_plus_one) );
  CHECK( parse_str("(10+1)")->equals(ten_plus_one) );
  CHECK( parse_str("(-10+1)")->equals(minus_ten_plus_one) );
  CHECK( parse_str("(10)+1")->equals(ten_plus_one) );
  CHECK( parse_str("(-10)+1")->equals(minus_ten_plus_one) );
  CHECK( parse_str("10+(1)")->equals(ten_plus_one) );
  CHECK( parse_str("-10+(1)")->equals(minus_ten_plus_one) );
  
  CHECK ( parse_str("42+2")->equals( NEW(AddExpr)(NEW(NumExpr)(42),
                                                 NEW(NumExpr)(2))) );
  CHECK ( parse_str("42+height")->equals( NEW(AddExpr)(NEW(NumExpr)(42),
                                                      NEW(VarExpr)("height"))) );
  CHECK ( parse_str("42+2+1")->equals( NEW(AddExpr)(NEW(NumExpr)(42),
                                                   NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1)))) );
  CHECK ( parse_str("42*2")->equals( NEW(MultExpr)(NEW(NumExpr)(42),
                                                  NEW(NumExpr)(2))) );
  CHECK ( parse_str("42*2+1")->equals( NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(42),
                                                                NEW(NumExpr)(2)), NEW(NumExpr)(1))) );
  CHECK ( parse_str("(42*2)+(3*2)")->equals( NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(42), NEW(NumExpr)(2)),
                                                         NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))) );
  CHECK ( parse_str("(hello*viralParrot)+(3*2)")->equals( NEW(AddExpr)(NEW(MultExpr)(NEW(VarExpr)("hello"),
                                                                                   NEW(VarExpr)("viralParrot")),
                                                                          NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))) );
  CHECK ( parse_str("(42* 2) * (3 + 2)+1")->equals( NEW(AddExpr)(NEW(MultExpr)(NEW(MultExpr)(NEW(NumExpr)(42), NEW(NumExpr)(2)),
                                                                             NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))), NEW(NumExpr)(1))) );
  CHECK ( parse_str("(42* 2) * (3 + 2)+indigo")->equals( NEW(AddExpr)(NEW(MultExpr)(NEW(MultExpr)(NEW(NumExpr)(42), NEW(NumExpr)(2)),
                                                                                  NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))), NEW(VarExpr)("indigo"))) );
  
  CHECK( parse_str("xyz")->equals(NEW(VarExpr)("xyz")) );
  CHECK( parse_str("xyz+1")->equals(NEW(AddExpr)(NEW(VarExpr)("xyz"), NEW(NumExpr)(1))) );
  
  CHECK( parse_str(" 10 ")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str(" -10 ")->equals(NEW(NumExpr)(-10)) );
  CHECK( parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str(" 10  + 1")->equals(ten_plus_one) );
  CHECK( parse_str(" ( 10 + 1 ) ")->equals(ten_plus_one) );
  CHECK( parse_str(" 11 * ( 10 + 1 ) ")->equals(NEW(MultExpr)(NEW(NumExpr)(11),
                                                             ten_plus_one)) );
  CHECK( parse_str(" ( 11 * 10 ) + 1 ")
        ->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11), NEW(NumExpr)(10)),
                             NEW(NumExpr)(1))) );
  CHECK( parse_str(" 1 + 2 * 3 ")
        ->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                             NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  
  CHECK( (parse_str("3+(-2)")
          ->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(-2)))) );
  CHECK( (parse_str("3+(-2)"))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(1)) );
  CHECK( (parse_str("3+(-2)"))->optimize()->to_string()
        == "1" );
  CHECK( (parse_str("3*(-2)")
          ->equals(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(-2)))) );
  CHECK( (parse_str("3*(-2)"))->interp(NEW(EmptyEnv)())
          ->equals(NEW(NumVal)(-6)) );
  CHECK( (parse_str("3*(-2)"))->optimize()->to_string()
        == "-6" );
  
  CHECK( parse_str("_let x = 3 _in x + 2")
        ->equals(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)))) );
  CHECK( parse_str("_let x = 3 _in x * 2")
        ->equals(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)))) );
  CHECK( (parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(LetExpr)("y", NEW(AddExpr)(NEW(VarExpr)("z"), NEW(NumExpr)(2)), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(AddExpr)(NEW(VarExpr)("y"), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))))))) );
  
  CHECK( parse_str("(_let x = 5 _in x) + 3")->optimize()->equals(NEW(NumExpr)(8)) );
  CHECK( parse_str("(_let x = 5 _in x) + 3")->optimize()->to_string() == "8" );
  CHECK_THROWS_WITH( parse_str("_let x = y + 3 + 5 _in x+4")->interp(NEW(EmptyEnv)())->to_string(), "free variable: y" );
  
  CHECK( parse_str("_let z =(_let x = 5 _in x) _in (z+1)")->optimize()->to_string() == "6" );
  
  CHECK( parse_str("3 == 3")->optimize()->to_string() == "_true" );
  CHECK( parse_str("3 == 4")->optimize()->to_string() == "_false" );
  CHECK( parse_str("2 + 1 == 3")->optimize()->to_string() == "_true" );
  CHECK( parse_str("-3 == -3")->optimize()->to_string() == "_true" );
  CHECK( parse_str("(2+(-1)) == 1")->optimize()->to_string() == "_true" );
  CHECK( parse_str("_true == _true")->optimize()->to_string() == "_true" );
  CHECK( parse_str("_false == _false")->optimize()->to_string() == "_true" );
  CHECK( parse_str("_false == _true")->optimize()->to_string() == "_false" );
  CHECK( parse_str("x == x")
        ->equals(NEW(CompExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))) );
  CHECK( parse_str("(2+(-1)) == 1")
        ->equals(NEW(CompExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(-1)), NEW(NumExpr)(1))) );
  
  CHECK( (parse_str("_if (5 == 5) _then 1 _else 2"))
        ->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)), NEW(NumExpr)(1), NEW(NumExpr)(2))) );
  CHECK( (parse_str("_if (4 == 5) _then 1 _else 2"))
        ->equals(NEW(IfExpr)(NEW(CompExpr)(NEW(NumExpr)(4), NEW(NumExpr)(5)), NEW(NumExpr)(1), NEW(NumExpr)(2))) );
  CHECK( (parse_str("_if (5 == 5) _then 1 _else 2"))->optimize()->to_string()
        == "1");
  CHECK( (parse_str("_if (4 == 5) _then 1 _else 2"))->optimize()->to_string()
        == "2");
  
  CHECK( (parse_str("_fun (x) x + 1"))
        ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)))) );
  CHECK( (parse_str("_fun (x) x + 1"))->to_string()
        == "(_fun (x) (x + 1))" );
  CHECK( (parse_str("_fun (x) (x + 1) (2)"))->to_string()
        == "(_fun (x) (x + 1) (2))" );
  CHECK( (parse_str("(_fun (x) (x + 1)) (2)"))
        ->equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))), NEW(NumExpr)(2))) );
  CHECK( (parse_str("(_fun (x) (x + 1)) (2)"))->interp(NEW(EmptyEnv)())
        ->equals(NEW(NumVal)(3)) );
  CHECK( (parse_str("(_fun (x) (x + 1)) (2)"))->interp(NEW(EmptyEnv)())->to_string()
        == "3" );
  CHECK( (parse_str("_let add = (_fun (x) (_fun (y) (x + y))) _in _let addFive = add(5) _in addFive(10)"))->interp(NEW(EmptyEnv)())->to_string()
        == "15" );
  CHECK( parse_str("_let f = _fun (x) x*8 _in f(2)")->interp(NEW(EmptyEnv)())->to_string()
        == "16" );
  CHECK( parse_str("_let y = 8 _in _let f = _fun (x) x*y _in f(2)")->interp(NEW(EmptyEnv)())->to_string()
        == "16");
  CHECK( parse_str("f(10)(1)")->equals(NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"),NEW(NumExpr)(10)),NEW(NumExpr)(1))) );
  CHECK( parse_str("_let fib = _fun (fib) _fun (x) _if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2) _in fib(fib)(10)")->interp(NEW(EmptyEnv)())->to_string()
                == "89" );
  
  PTR(Expr) fact = parse_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in _let factorial = factrl(factrl) _in factorial(5)");
  CHECK( fact->interp(NEW(EmptyEnv)())->to_string()
        == "120");
  
  CHECK( parse_str("f(10)(1)")->equals(NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(10)), NEW(NumExpr)(1))) );
  
  CHECK( parse_str("(_true)")->equals(NEW(BoolExpr)(true)) );
  CHECK( parse_str("(_false)")->equals(NEW(BoolExpr)(false)) );
  CHECK( (parse_str("(_true+1)")->equals(NEW(AddExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(1)))) );
}
