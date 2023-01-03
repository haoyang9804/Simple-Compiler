#include "../src/compiler.cpp"
#include <iostream>
#include <vector>
#include <string>

int main() {
  
  /*
  let b = 1 in
    let c = 2 * 3 in
      let a = fn(x, y){x + y} in
        a(b, c)
  */
  
  
  Expr::Var* var1 = new Expr::Var("a");
  Expr::Var* var2 = new Expr::Var("b");
  Expr::Var* var3 = new Expr::Var("c");
  
  Expr::App* app1 = new Expr::App(var1, std::vector<std::string>{"b", "c"}); // a(b, c)
 
  Expr::Var* var4 = new Expr::Var("x");
  Expr::Var* var5 = new Expr::Var("y");
  Expr::Add* add1 = new Expr::Add(var4, var5);
  Expr::Fn* fn1 = new Expr::Fn(std::vector<std::string>{"x", "y"}, add1); // fn(x, y) {x + y}

  /*
    let a = fn(x, y){x + y} in
      a(a, b)
  */
  Expr::Let* let1 = new Expr::Let("a", fn1, app1); 

  Expr::Cst* cst1 = new Expr::Cst(1);
  Expr::Cst* cst2 = new Expr::Cst(2);
  Expr::Cst* cst3 = new Expr::Cst(3);
  Expr::Mul* mul1 = new Expr::Mul(cst2, cst3);
  
  /*
    let c = 2 * 3 in
      let a = fn(x, y){x + y} in
        a(b, c) 
  */
  Expr::Let* let2 = new Expr::Let("c", mul1, let1);

  /*
    let b = 1 in
      let c = 2 * 3 in
        let a = fn(x, y){x + y} in
          a(b, c)
  */
  Expr::Let* let3 = new Expr::Let("b", cst1, let2);
  
  std::cout << "Expression is \"" << Expr::to_str(let3) << "\"" << std::endl;
  std::cout << "eval should be " << Expr::eval_final(let3, {}) << std::endl;

}