open Interpreter.Interp0
open Interpreter.Interp0.Expr
open Core

let expr = Add (Add (Cst 1, Cst 2), Mul (Cst 3, Cst 4))

let () =
  Printf.printf "testCompiler0:\n";
  let instrs = compile expr in
  Instruction.print_instrs instrs
