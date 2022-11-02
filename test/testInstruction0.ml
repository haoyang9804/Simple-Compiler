open Interpreter.Interp0.Instruction
open Core

let instrs : instrs = [ Cst 1; Cst 2; Add ]
let stack : stack = []

let () =
  Printf.printf "testInstruction0:\n";
  let v = eval instrs stack in
  print_operand v
