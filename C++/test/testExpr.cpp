#include "../src/interp.cpp"
#include <iostream>

using namespace Expr;

int main() {
  Cst* cst1 = new Cst(2);
  Var* var1 = new Var("var1");
  Add* add1 = new Add(cst1, var1);
  Cst* cst2 = new Cst(4);
  Mul* mul1 = new Mul(add1, cst2);
  Cst* cst3 = new Cst(5);
  Let* let1 = new Let("var1", cst3, mul1);
  if (eval(let1, {}) != 28) {
    throw std::logic_error("eval(let1, {} is not 28");
  }
  std::cout << "eval shold be 28: " << eval(let1, {}) << std::endl;
}