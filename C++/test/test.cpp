#include "../src/compiler.cpp"
#include <iostream>


int main() {


  Expr::Cst* cst1 = new Expr::Cst(2);
  Expr::Var* var1 = new Expr::Var("var1");
  Expr::Add* add1 = new Expr::Add(cst1, var1);
  Expr::Cst* cst2 = new Expr::Cst(4);
  Expr::Mul* mul1 = new Expr::Mul(add1, cst2);
  Expr::Cst* cst3 = new Expr::Cst(5);
  Expr::Let* let1 = new Expr::Let("var1", cst3, mul1);
  
  // test Expr
  if (Expr::eval(let1, {}) != 28) {
    throw std::logic_error("eval(let1, {} is not 28");
  }
  std::cout << "Expression is \"" << Expr::to_str(let1) << "\"" << std::endl;
  std::cout << "eval shold be 28: " << Expr::eval(let1, {}) << std::endl;

  // test Nameless
  Nameless::Expr* nLet1 = Compiler::lowerFromExprToNameless(let1, {});
  if (Nameless::eval(nLet1, {}) != 28) {
    throw std::logic_error("eval(let1, {} is not 28");
  } 
  std::cout << "Expression is \"" << Nameless::to_str(nLet1) << "\"" << std::endl;
  std::cout << "eval should be 28: " << Nameless::eval(nLet1, {}) << std::endl; 
}