open Interpreter.Interp.Instruction

let instrs : instrs = [Cst 1; Cst 2; Add]
let stack : stack = []

let () = 
  let v = eval instrs stack in
  print_operand v