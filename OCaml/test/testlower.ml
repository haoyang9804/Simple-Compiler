open Core
open Interpreter.Interp1

(* let expr = Expr.Add (Expr.Let ("x", Expr.Let("y", Cst 1, Var "y"), Expr.Mul (Var "x", Cst 2)), Cst 4) *)
let expr = Expr.Let ("x", Expr.Cst 17, Expr.Add (Expr.Var "x", Expr.Var "x"))
let expr_value = Expr.eval expr Expr.empty_env
let lower_expr = lower expr ([] : cenv)
let lower_expr_value = Nameless.eval lower_expr Nameless.empty_env
let indexed_expr = compileNamelessIntoIndexed lower_expr
let instrs = compileIndexedIntoInstruction indexed_expr
let instrs2 = NamelesstoStackVM.compile lower_expr
let instrs_value = Instruction.eval instrs []
let instrs2_value = Instruction.eval instrs2 []

let () =
  Printf.printf "testlower:\n";
  Printf.printf "expr is ";
  Expr.print_expr expr;
  Printf.printf "expr_value is %d\n" expr_value;
  Printf.printf "lower_expr is ";
  Nameless.print_expr lower_expr;
  Printf.printf "lower_expr_value is %d\n" lower_expr_value;
  Printf.printf "instrs_value is %d\n" instrs_value;
  Printf.printf "instrs2_value is %d\n" instrs2_value
