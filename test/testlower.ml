open Core
open Interpreter.Interp1

let expr = Expr.Add (Expr.Let ("x", Expr.Let("y", Cst 1, Var "y"), Expr.Mul (Var "x", Cst 2)), Cst 4)
let expr_value = Expr.eval expr Expr.empty_env
let lower_expr = lower expr ([] : cenv)

let lower_expr_value = Nameless.eval lower_expr Nameless.empty_env

let () = Printf.printf "testlower:\n";
Printf.printf "expr_value is %d\n" expr_value;
Printf.printf "lower_expr_value is %d\n" lower_expr_value