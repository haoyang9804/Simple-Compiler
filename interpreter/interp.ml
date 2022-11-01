open Core
module Expr = struct
  type expr = Cst of int | Add of expr * expr | Mul of expr * expr

  let rec eval = function
    | Cst i -> i
    | Add (a, b) -> eval a + eval b
    | Mul (a, b) -> eval a + eval b
end

module Instruction = struct
  type instr = Cst of int | Add | Mul
  type instrs = instr list
  type operand = int
  type stack = operand list

  let print_stack stack =
    List.iter ~f:(Printf.printf "%d ") stack

  let rec eval (instrs : instrs) (stack : stack) =
    match (instrs, stack) with
    | (Cst i :: tail, _) -> eval tail (i :: stack)
    | (Add :: tail, a :: b :: stack_) -> eval tail (a + b :: stack_)
    | (Mul :: tail, a :: b :: stack_) -> eval tail (a * b :: stack_)
    | ([], _) -> stack
    | _ -> failwith "Bad instructions or stack"

end