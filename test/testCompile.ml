open Interpreter.Interp
open Interpreter.Interp.Expr
let expr = Add (Add (Cst(1), Cst (2)), Mul (Cst (3), Cst(4)))

let () = let instrs = compile expr in
  Instruction.print_instrs instrs