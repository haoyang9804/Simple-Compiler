#include "../src/compiler.cpp"
#include <iostream>

int main() {

  Expr::Cst *cst1 = new Expr::Cst(2);
  Expr::Var *var1 = new Expr::Var("var1");
  Expr::Add *add1 = new Expr::Add(var1, cst1);
  Expr::Cst *cst2 = new Expr::Cst(4);
  Expr::Mul *mul1 = new Expr::Mul(add1, cst2);
  Expr::Cst *cst3 = new Expr::Cst(5);
  Expr::Let *let1 = new Expr::Let("var1", cst3, mul1);
  Expr::Var *var2 = new Expr::Var("var2");
  Expr::Mul *mul2 = new Expr::Mul(var2, let1);
  Expr::Cst *cst4 = new Expr::Cst(2);
  Expr::Let *let2 = new Expr::Let("var2", cst4, mul2);

  Expr::Cst *cst5 = new Expr::Cst(5);
  Expr::Cst *cst6 = new Expr::Cst(6);
  Expr::Add *add2 = new Expr::Add(cst5, cst6);

  // test Expr 1
  {
    std::cout << "=====test Expr 1=====" << std::endl;
    std::cout << "Expression is \"" << Expr::to_str(let2) << "\"" << std::endl;
    std::cout << "eval shold be: " << Expr::eval(let2, {}) << std::endl;
  }

  // test Nameless 1
  {
    std::cout << "=====test Nameless 1=====" << std::endl;
    Nameless::Expr *nLet2 = Compiler::lowerFromExprToNameless(let2, {});
    std::cout << "Nameless expression is \"" << Nameless::to_str(nLet2) << "\""
              << std::endl;
    std::cout << "eval should be: " << Nameless::eval(nLet2, {}) << std::endl;
  }

  // test Instruction 1
  {
    std::cout << "=====test Instruction 1=====" << std::endl;
    Nameless::Expr *nLet2 = Compiler::lowerFromExprToNameless(let2, {});
    Instruction::InstrPtrs instrPtrs =
        Compiler::lowerFromNamelessToInstruction(nLet2, {});
    std::cout << "Instructions are\n" << Instruction::to_str(instrPtrs);
    std::cout << "eval should be: " << Instruction::eval(instrPtrs, {})
              << std::endl;
  }

  // test Instruction 2
  {
    std::cout << "=====test Instruction 2=====" << std::endl;
    Instruction::InstrPtrs instrPtrs = {new Instruction::Cst(1)};
    std::cout << "Instructions are\n" << Instruction::to_str(instrPtrs);
    std::cout << "eval should be: " << Instruction::eval(instrPtrs, {})
              << std::endl;
  }

  // test Instruction 3
  {
    std::cout << "=====test Instruction 3=====" << std::endl;
    Nameless::Expr *nAdd2 = Compiler::lowerFromExprToNameless(add2, {});
    Instruction::InstrPtrs instrPtrs =
        Compiler::lowerFromNamelessToInstruction(nAdd2, {});
    std::cout << "Instructions are\n" << Instruction::to_str(instrPtrs);
    std::cout << "eval should be: " << Instruction::eval(instrPtrs, {})
              << std::endl;
  }
}