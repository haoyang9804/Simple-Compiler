open Interpreter.Interp1.Expr
open Core

let () =
  Printf.printf "testExpr1:\n";
  Printf.printf "%d\n" (eval (Let ("x", Cst 1, Var "x")) empty_env)
