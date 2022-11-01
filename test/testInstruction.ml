open Interpreter.Interp.Instruction

let instrs : instrs = [Cst 1; Cst 2; Add]
let stack : stack = []

let () = 
  let stk = eval instrs stack in
  print_stack stk