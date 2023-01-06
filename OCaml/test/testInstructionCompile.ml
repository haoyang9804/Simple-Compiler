open Core
open Interpreter.Interp1.Instruction

let instrs = [ Cst 17; Var 0; Var 1; Mul; Swap; Pop ]
let value = eval instrs []

let () =
  Printf.printf "testInsturctionCompile:\n";
  Printf.printf "%d\n" value
