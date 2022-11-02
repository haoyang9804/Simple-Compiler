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
    List.iter ~f:(Printf.printf "%d ") stack;
    Printf.printf "\n"
  
  let instr_to_string = function
    | Cst i -> string_of_int i
    | Add -> "Add"
    | Mul -> "Mul"
  
  let print_instr instr = print_string @@ instr_to_string instr

  let print_instrs instrs =
    List.iter ~f:(fun instr -> print_instr instr; Printf.printf " ") instrs;
    Printf.printf "\n"
  
  let print_operand = Printf.printf "%d\n"

  let rec eval (instrs : instrs) (stack : stack) =
    match (instrs, stack) with
    | (Cst i :: tail, _) -> eval tail (i :: stack)
    | (Add :: tail, a :: b :: stack_) -> eval tail (a + b :: stack_)
    | (Mul :: tail, a :: b :: stack_) -> eval tail (a * b :: stack_)
    | ([], v :: []) -> v
    | _ -> failwith "Bad instructions or stack"
end

let rec compile (expr : Expr.expr)=
  match expr with
  | Expr.Cst i -> [Instruction.Cst i]
  | Add (e1, e2) -> compile e1 @ compile e2 @ [Add]
  | Mul (e1, e2) -> compile e1 @ compile e2 @ [Mul]