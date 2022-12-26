#include "../src/compiler.cpp"
#include <iostream>


int main() {


  Expr::Cst* cst1 = new Expr::Cst(2);
  Expr::Var* var1 = new Expr::Var("var1");
  Expr::Add* add1 = new Expr::Add(var1, cst1);
  Expr::Cst* cst2 = new Expr::Cst(4);
  Expr::Mul* mul1 = new Expr::Mul(add1, cst2);
  Expr::Cst* cst3 = new Expr::Cst(5);
  Expr::Let* let1 = new Expr::Let("var1", cst3, mul1);
  Expr::Var* var2 = new Expr::Var("var2");
  Expr::Mul* mul2 = new Expr::Mul(var2, let1);
  Expr::Cst* cst4 = new Expr::Cst(2);
  Expr::Let* let2 = new Expr::Let("var2", cst4, mul2);
  
  // test Expr
  std::cout << "Expression is \"" << Expr::to_str(let2) << "\"" << std::endl;
  std::cout << "eval shold be: " << Expr::eval(let2, {}) << std::endl;

  // test Nameless
  Nameless::Expr* nLet2 = Compiler::lowerFromExprToNameless(let2, {});
  std::cout << "Expression is \"" << Nameless::to_str(nLet2) << "\"" << std::endl;
  std::cout << "eval should be: " << Nameless::eval(nLet2, {}) << std::endl; 
}