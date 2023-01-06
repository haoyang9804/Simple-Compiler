#include "../src/compiler.cpp"
#include <iostream>
#include <string>
#include <vector>

int main() {

  {
    // test 1 for Expr and Nameless
    /*
    let b = 1 in
      let c = 2 * 3 in
        let a = fn(x, y){x + y} in
          a(b, c)
    */
    std::cout << "========== Test 1 ==========" << std::endl;

    Expr::Var *var1 = new Expr::Var("a");
    Expr::Var *var2 = new Expr::Var("b");
    Expr::Var *var3 = new Expr::Var("c");

    Expr::App *app1 = new Expr::App(
        var1, std::vector<Expr::STRING_OR_EXPR>{"b", "c"}); // a(b, c)

    Expr::Var *var4 = new Expr::Var("x");
    Expr::Var *var5 = new Expr::Var("y");
    Expr::Add *add1 = new Expr::Add(var4, var5);
    Expr::Fn *fn1 = new Expr::Fn(std::vector<std::string>{"x", "y"},
                                 add1); // fn(x, y) {x + y}

    /*
      let a = fn(x, y){x + y} in
        a(a, b)
    */
    Expr::Let *let1 = new Expr::Let("a", fn1, app1);

    Expr::Cst *cst1 = new Expr::Cst(1);
    Expr::Cst *cst2 = new Expr::Cst(2);
    Expr::Cst *cst3 = new Expr::Cst(3);
    Expr::Mul *mul1 = new Expr::Mul(cst2, cst3);

    /*
      let c = 2 * 3 in
        let a = fn(x, y){x + y} in
          a(b, c)
    */
    Expr::Let *let2 = new Expr::Let("c", mul1, let1);

    /*
      let b = 1 in
        let c = 2 * 3 in
          let a = fn(x, y){x + y} in
            a(b, c)
    */
    Expr::Let *let3 = new Expr::Let("b", cst1, let2);

    std::cout << "Expression is \"" << Expr::to_str(let3) << "\"" << std::endl;
    std::cout << "eval should be 7, and the calculation result is "
              << Expr::eval_final(let3, {}) << std::endl;
    
    std::cout << "=====Lowering to Nameless Expression=====" << std::endl;
    Nameless::Expr *nlet3 = Compiler::lowerFromExprToNameless(let3, {});
    std::cout << "Expression is \"" << Nameless::to_str(nlet3) << "\"" << std::endl;
    std::cout << "eval should be 7, and the calculation result is " << Nameless::eval_final(nlet3, {}) << std::endl;
  }

  {
    // test 2 for Expr and Nameless
    /*
    let b = 1 in
        let a = fn(x, y){x + y} in
          a(b, (2*3))
    */

    std::cout << "========== Test 2 ==========" << std::endl;

    Expr::Var *var1 = new Expr::Var("a");
    Expr::Var *var2 = new Expr::Var("b");
    Expr::Var *var3 = new Expr::Var("c");

    Expr::Cst *cst2 = new Expr::Cst(2);
    Expr::Cst *cst3 = new Expr::Cst(3);
    Expr::Mul *mul1 = new Expr::Mul(cst2, cst3); // 2 * 3

    Expr::App *app1 = new Expr::App(
        var1, std::vector<Expr::STRING_OR_EXPR>{"b", mul1}); // a(b, (2*3))

    Expr::Var *var4 = new Expr::Var("x");
    Expr::Var *var5 = new Expr::Var("y");
    Expr::Add *add1 = new Expr::Add(var4, var5);
    Expr::Fn *fn1 = new Expr::Fn(std::vector<std::string>{"x", "y"},
                                 add1); // fn(x, y) {x + y}

    /*
      let a = fn(x, y){x + y} in
        a(b, (2*3))
    */
    Expr::Let *let1 = new Expr::Let("a", fn1, app1);

    Expr::Cst *cst1 = new Expr::Cst(1);

    /*
      let b = 1 in
          let a = fn(x, y){x + y} in
            a(b, c)
    */
    Expr::Let *let3 = new Expr::Let("b", cst1, let1);
    std::cout << "Expression is \"" << Expr::to_str(let3) << "\"" << std::endl;
    std::cout << "eval should be 7, and the calculation result is "
              << Expr::eval_final(let3, {}) << std::endl;
    
    std::cout << "=====Lowering to Nameless Expression=====" << std::endl;
    Nameless::Expr *nlet3 = Compiler::lowerFromExprToNameless(let3, {});
    std::cout << "Expression is \"" << Nameless::to_str(nlet3) << "\"" << std::endl;
    std::cout << "eval should be 7, and the calculation result is " << Nameless::eval_final(nlet3, {}) << std::endl;
  }

  {
    // Test 3

    std::cout << "========== Test 3 ==========" << std::endl;
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

    std::cout << "Expression is \"" << Expr::to_str(let2) << "\"" << std::endl;
    std::cout << "eval shold be: " << Expr::eval_final(let2, {}) << std::endl;

    std::cout << "=====Lowering to Nameless Expression=====" << std::endl;
    Nameless::Expr *nLet2 = Compiler::lowerFromExprToNameless(let2, {});
    std::cout << "Nameless expression is \"" << Nameless::to_str(nLet2) << "\""
              << std::endl;
    std::cout << "eval should be: " << Nameless::eval_final(nLet2, {}) << std::endl;
  }

}