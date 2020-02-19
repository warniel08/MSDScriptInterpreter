//
//  parse.cpp
//  HW03ArithParser_updated
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#include "parse.hpp"
#include "catch.hpp"

#include <iostream>
#include <sstream>

/*
 * Functions that return the expression objects
 * */
static Expr *parse_expr(std::istream &in);
static Expr *parse_comparg(std::istream &in);
static Expr *parse_addend(std::istream &in);
static Expr *parse_multicand(std::istream &in);
static Expr *parse_inner(std::istream &in);
static Expr *parse_number(std::istream &in);
static Expr *parse_variable(std::istream &in);
static std::string parse_keyword(std::istream &in);
static std::string parse_alphabetic(std::istream &in, std::string prefix);
static char peek_after_spaces(std::istream &in);
static Expr *parse_let(std::istream &in);
static Expr *parse_if(std::istream &in);
static Expr *parse_fun(std::istream &in);

// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse errors.
Expr *parse(std::istream &in) {
  Expr *e = parse_expr(in);
  
  char c = peek_after_spaces(in);
  if (!in.eof())
    throw std::runtime_error((std::string)"expected end of file at " + c);
  
  return e;
}

// Takes an input stream that starts with an expression,
// consuming the largest initial expression possible.
static Expr *parse_expr(std::istream &in) {
  Expr *e = parse_comparg(in);
  
  char c = peek_after_spaces(in);
  if (c == '=') {
    c = in.get();
    c = in.get();
    if (c == '=') {
      Expr *rhs = parse_expr(in);
      e = new CompExpr(e, rhs);
    } else
       throw std::runtime_error("not a comp expr");
  }
  
  return e;
}

static Expr *parse_comparg(std::istream &in) {
  Expr *e = parse_addend(in);
  
  char c = peek_after_spaces(in);
  if (c == '+') {
    in >> c;
    Expr *rhs = parse_comparg(in);
    e = new AddExpr(e, rhs);
  }
  
  return e;
}

// Takes an input stream that starts with an addend,
// consuming the largest initial addend possible, where
// an addend is an expression that does not have `+`
// except within nested expressions (like parentheses).
static Expr *parse_addend(std::istream &in) {
  Expr *e = parse_multicand(in);
  
  char c = peek_after_spaces(in);
  if (c == '*') {
    c = in.get();
    Expr *rhs = parse_addend(in);
    e = new MultExpr(e, rhs);
  }
  
  return e;
}

static Expr *parse_multicand(std::istream &in) {
  Expr *e = parse_inner(in);
  
  while (peek_after_spaces(in) == '(') {
    in.get();
    Expr *actual_arg = parse_expr(in);
    e = new CallExpr(e, actual_arg);
    if (peek_after_spaces(in) == ')')
      in.get();
    else
      throw std::runtime_error("need a )");
  }
  
  return e;
}

// Parses something with no immediate `+` or `*` from `in`.
static Expr *parse_inner(std::istream &in) {
  Expr *e;
  
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
      return new BoolExpr(false);
    else if (keyword == "_true")
      return new BoolExpr(true);
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
static Expr *parse_number(std::istream &in) {
  char c = peek_after_spaces(in);
  if (c == '-')
    c = in.get();
  int num = 0;
  in >> num;
  if (c == '-')
    num *= -1;
  return new NumExpr(num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
static Expr *parse_variable(std::istream &in) {
  return new VarExpr(parse_alphabetic(in, ""));
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
static Expr *parse_let(std::istream &in) {
  char c;
  c = peek_after_spaces(in);
  std::string retParseAlphaName = parse_alphabetic(in, "");
  
  c = peek_after_spaces(in);
  in.get();
  
  c = peek_after_spaces(in);
  Expr *rhs = parse_expr(in);
  
  c = peek_after_spaces(in);
  std::string pKeyWord = parse_keyword(in);
  
  c = peek_after_spaces(in);
  Expr *body = parse_expr(in);
  
  Expr *newLet = new LetExpr(retParseAlphaName, rhs, body);
  
  return newLet;
}

static Expr *parse_if(std::istream &in) {
  Expr *test_part = parse_expr(in);
  std::string then_clause = parse_keyword(in);
  
  if (then_clause != "_then")
    throw std::runtime_error("expected a _then keyword");
  
  Expr *then_part = parse_expr(in);
  std::string else_clause = parse_keyword(in);
  
  if (else_clause != "_else")
    throw std::runtime_error("expected a _else keyword");
  
  Expr *else_part = parse_expr(in);
  
  return new IfExpr(test_part, then_part, else_part);
}

static Expr *parse_fun(std::istream &in) {
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
  
  Expr *body = parse_expr(in);
  
  return new FunExpr(formal_arg, body);
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
static Expr *parse_str(std::string s) {
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
  Expr *ten_plus_one = new AddExpr(new NumExpr(10), new NumExpr(1));
  Expr *minus_ten_plus_one = new AddExpr(new NumExpr(-10), new NumExpr(1));
  
  CHECK ( parse_str_error(" (1 ") == "expected a close parenthesis" );
  CHECK ( parse_str_error(" ( 1 ") == "expected a close parenthesis" );
  CHECK ( parse_str_error(" _ ") == "unexpected keyword _");
  CHECK ( parse_str_error("!") == "expected a digit or open parenthesis or letter at !" );
  CHECK ( parse_str_error("(1") == "expected a close parenthesis" );
  CHECK ( parse_str_error(" ! ") == "expected a digit or open parenthesis or letter at !" );
  CHECK ( parse_str_error(" 1 )") == "expected end of file at )" );
  CHECK ( parse_str_error(" 7 = 5") == "not a comp expr" );
  CHECK ( parse_str_error(" (_fun 5)") == "expected a ( paren" );
  
  CHECK( parse_str("10")->equals(new NumExpr(10)) );
  CHECK( parse_str("-10")->equals(new NumExpr(-10)) );
  CHECK( parse_str("(10)")->equals(new NumExpr(10)) );
  CHECK( parse_str("(-10)")->equals(new NumExpr(-10)) );
  CHECK( parse_str("10+1")->equals(ten_plus_one) );
  CHECK( parse_str("-10+1")->equals(minus_ten_plus_one) );
  CHECK( parse_str("(10+1)")->equals(ten_plus_one) );
  CHECK( parse_str("(-10+1)")->equals(minus_ten_plus_one) );
  CHECK( parse_str("(10)+1")->equals(ten_plus_one) );
  CHECK( parse_str("(-10)+1")->equals(minus_ten_plus_one) );
  CHECK( parse_str("10+(1)")->equals(ten_plus_one) );
  CHECK( parse_str("-10+(1)")->equals(minus_ten_plus_one) );
  
  CHECK ( parse_str("42+2")->equals( new AddExpr(new NumExpr(42),
                                                 new NumExpr(2))) );
  CHECK ( parse_str("42+height")->equals( new AddExpr(new NumExpr(42),
                                                      new VarExpr("height"))) );
  CHECK ( parse_str("42+2+1")->equals( new AddExpr(new NumExpr(42),
                                                   new AddExpr(new NumExpr(2), new NumExpr(1)))) );
  CHECK ( parse_str("42*2")->equals( new MultExpr(new NumExpr(42),
                                                  new NumExpr(2))) );
  CHECK ( parse_str("42*2+1")->equals( new AddExpr(new MultExpr(new NumExpr(42),
                                                                new NumExpr(2)), new NumExpr(1))) );
  CHECK ( parse_str("(42*2)+(3*2)")->equals( new AddExpr(new MultExpr(new NumExpr(42), new NumExpr(2)),
                                                         new MultExpr(new NumExpr(3), new NumExpr(2)))) );
  CHECK ( parse_str("(hello*viralParrot)+(3*2)")->equals( new AddExpr(new MultExpr(new VarExpr("hello"), new VarExpr("viralParrot")),
                                                                      new MultExpr(new NumExpr(3), new NumExpr(2)))) );
  CHECK ( parse_str("(42* 2) * (3 + 2)+1")->equals( new AddExpr(new MultExpr(new MultExpr(new NumExpr(42), new NumExpr(2)),
                                                                             new AddExpr(new NumExpr(3), new NumExpr(2))), new NumExpr(1))) );
  CHECK ( parse_str("(42* 2) * (3 + 2)+indigo")->equals( new AddExpr(new MultExpr(new MultExpr(new NumExpr(42), new NumExpr(2)),
                                                                                  new AddExpr(new NumExpr(3), new NumExpr(2))), new VarExpr("indigo"))) );
  
  CHECK( parse_str("xyz")->equals(new VarExpr("xyz")) );
  CHECK( parse_str("xyz+1")->equals(new AddExpr(new VarExpr("xyz"), new NumExpr(1))) );
  
  CHECK( parse_str(" 10 ")->equals(new NumExpr(10)) );
  CHECK( parse_str(" -10 ")->equals(new NumExpr(-10)) );
  CHECK( parse_str(" (  10 ) ")->equals(new NumExpr(10)) );
  CHECK( parse_str(" 10  + 1")->equals(ten_plus_one) );
  CHECK( parse_str(" ( 10 + 1 ) ")->equals(ten_plus_one) );
  CHECK( parse_str(" 11 * ( 10 + 1 ) ")->equals(new MultExpr(new NumExpr(11),
                                                             ten_plus_one)) );
  CHECK( parse_str(" ( 11 * 10 ) + 1 ")
        ->equals(new AddExpr(new MultExpr(new NumExpr(11), new NumExpr(10)),
                             new NumExpr (1))) );
  CHECK( parse_str(" 1 + 2 * 3 ")
        ->equals(new AddExpr(new NumExpr(1),
                             new MultExpr(new NumExpr(2), new NumExpr(3)))) );
  
  CHECK( (parse_str("3+(-2)")
          ->equals(new AddExpr(new NumExpr(3), new NumExpr(-2)))) );
  CHECK( (parse_str("3+(-2)"))->interp()
          ->equals(new NumVal(1)) );
  CHECK( (parse_str("3+(-2)"))->optimize()->to_string()
        == "1" );
  CHECK( (parse_str("3*(-2)")
          ->equals(new MultExpr(new NumExpr(3), new NumExpr(-2)))) );
  CHECK( (parse_str("3*(-2)"))->interp()
          ->equals(new NumVal(-6)) );
  CHECK( (parse_str("3*(-2)"))->optimize()->to_string()
        == "-6" );
  
  CHECK( parse_str("_let x = 3 _in x + 2")
        ->equals(new LetExpr("x", new NumExpr(3), new AddExpr(new VarExpr("x"), new NumExpr(2)))) );
  CHECK( parse_str("_let x = 3 _in x * 2")
        ->equals(new LetExpr("x", new NumExpr(3), new MultExpr(new VarExpr("x"), new NumExpr(2)))) );
  CHECK( (parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")
          ->equals(new LetExpr("x", new NumExpr(5), new LetExpr("y", new AddExpr(new VarExpr("z"), new NumExpr(2)), new AddExpr(new VarExpr("x"), new AddExpr(new VarExpr("y"), new MultExpr(new NumExpr(2), new NumExpr(3)))))))) );
  
  CHECK( parse_str("(_let x = 5 _in x) + 3")->optimize()->equals(new NumExpr(8)) );
  CHECK( parse_str("(_let x = 5 _in x) + 3")->optimize()->to_string() == "8" );
  CHECK( parse_str("_let z =(_let x = 5 _in x) _in (z+1)")->optimize()->to_string() == "6" );
  
  CHECK( parse_str("3 == 3")->optimize()->to_string() == "(_true)" );
  CHECK( parse_str("3 == 4")->optimize()->to_string() == "(_false)" );
  CHECK( parse_str("2 + 1 == 3")->optimize()->to_string() == "(_true)" );
  CHECK( parse_str("-3 == -3")->optimize()->to_string() == "(_true)" );
  CHECK( parse_str("(2+(-1)) == 1")->optimize()->to_string() == "(_true)" );
  CHECK( parse_str("_true == _true")->optimize()->to_string() == "(_true)" );
  CHECK( parse_str("_false == _false")->optimize()->to_string() == "(_true)" );
  CHECK( parse_str("_false == _true")->optimize()->to_string() == "(_false)" );
  CHECK( parse_str("x == x")
        ->equals(new CompExpr(new VarExpr("x"), new VarExpr("x"))) );
  CHECK( parse_str("(2+(-1)) == 1")
        ->equals(new CompExpr(new AddExpr(new NumExpr(2), new NumExpr(-1)), new NumExpr(1))) );
  
  CHECK( (parse_str("_if (5 == 5) _then 1 _else 2"))
        ->equals(new IfExpr(new CompExpr(new NumExpr(5), new NumExpr(5)), new NumExpr(1), new NumExpr(2))) );
  CHECK( (parse_str("_if (4 == 5) _then 1 _else 2"))
        ->equals(new IfExpr(new CompExpr(new NumExpr(4), new NumExpr(5)), new NumExpr(1), new NumExpr(2))) );
  CHECK( (parse_str("_if (5 == 5) _then 1 _else 2"))->optimize()->to_string()
        == "1");
  CHECK( (parse_str("_if (4 == 5) _then 1 _else 2"))->optimize()->to_string()
        == "2");
  
  CHECK( (parse_str("_fun (x) x + 1"))
        ->equals(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(1)))) );
  CHECK( (parse_str("_fun (x) x + 1"))->to_string()
        == "(_fun (x) (x + 1))" );
  CHECK( (parse_str("_fun (x) (x + 1) (2)"))->to_string()
        == "(_fun (x) (x + 1) (2))" );
  CHECK( (parse_str("(_fun (x) (x + 1)) (2)"))
        ->equals(new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(1))), new NumExpr(2))) );
  CHECK( (parse_str("(_fun (x) (x + 1)) (2)"))->interp()
        ->equals(new NumVal(3)) );
  CHECK( (parse_str("(_fun (x) (x + 1)) (2)"))->interp()->to_string()
        == "3" );
  CHECK( (parse_str("_let add = (_fun (x) (_fun (y) (x + y))) _in _let addFive = add(5) _in addFive(10)"))->interp()->to_string()
        == "15" );
  CHECK( parse_str("_let f = _fun (x) x*8 _in f(2)")->interp()->to_string()
        == "16" );
  CHECK( parse_str("_let y = 8 _in _let f = _fun (x) x*y _in f(2)")->interp()->to_string()
        == "16");
  CHECK( parse_str("f(10)(1)")->equals(new CallExpr(new CallExpr(new VarExpr("f"),new NumExpr(10)),new NumExpr(1))));
  CHECK( parse_str("_let fib = _fun (fib) _fun (x) _if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2) _in fib(fib)(10)")->interp()->to_string()
                == "89" );
  
  Expr *fact = parse_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in _let factorial = factrl(factrl) _in factorial(5)");
  CHECK( fact->interp()->to_string()
        == "120");
  
  CHECK(parse_str("f(10)(1)")->equals(new CallExpr(new CallExpr(new VarExpr("f"),new NumExpr(10)),new NumExpr(1))));
  
  CHECK( parse_str("(_true)")->equals(new BoolExpr(true)) );
  CHECK( parse_str("(_false)")->equals(new BoolExpr(false)) );
  CHECK( (parse_str("(_true+1)")->equals(new AddExpr(new BoolExpr(true), new NumExpr(1)))) );
}
