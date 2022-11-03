open Core

module Expr = struct
  type expr =
    | Cst of int
    | Add of expr * expr
    | Mul of expr * expr
    | Var of string
    | Let of string * expr * expr

  let rec expr2string = function
    | Cst i -> String.concat ~sep:" " ["Cst"; string_of_int i]
    | Add (e1, e2) -> String.concat ~sep:" " ["Add"; expr2string e1; expr2string e2]
    | Mul (e1, e2) -> String.concat ~sep:" " ["Mul"; expr2string e1; expr2string e2]
    | Var x -> String.concat ~sep:" " ["Var"; x]
    | Let (x, e1, e2) -> String.concat ~sep:" " ["Let"; x; expr2string e1; expr2string e2]
  
  let print_expr expr =
    let expr_str = expr2string expr
    in Printf.printf "%s\n" expr_str

  type env = (string, int, String.comparator_witness) Map.t

  let empty_env = Map.empty (module String)

  let rec eval expr (env : env) =
    match expr with
    | Cst i -> i
    | Add (e1, e2) -> eval e1 env + eval e2 env
    | Mul (e1, e2) -> eval e1 env * eval e2 env
    | Var str -> Map.find_exn env str
    | Let (x, e1, e2) -> eval e2 (Map.add_exn env ~key:x ~data:(eval e1 env))
end

module Nameless = struct
  type expr =
    | Cst of int
    | Add of expr * expr
    | Mul of expr * expr
    | Var of int
    | Let of expr * expr

  let rec expr2string = function
    | Cst i -> String.concat ~sep:" " ["Cst"; string_of_int i]
    | Add (e1, e2) -> String.concat ~sep:" " ["Add"; expr2string e1; expr2string e2]
    | Mul (e1, e2) -> String.concat ~sep:" " ["Mul"; expr2string e1; expr2string e2]
    | Var i -> String.concat ~sep:" " ["Var"; string_of_int i]
    | Let (e1, e2) -> String.concat ~sep:" " ["Let"; expr2string e1; expr2string e2]
  
  let print_expr expr =
    let expr_str = expr2string expr
    in Printf.printf "%s\n" expr_str

  type env = int list

  let empty_env:env = []

  let rec eval expr (env : env) =
    match expr with
    | Cst i -> i
    | Add (e1, e2) -> eval e1 env + eval e2 env
    | Mul (e1, e2) -> eval e1 env * eval e2 env
    | Var i -> List.nth_exn env i
    | Let (e1, e2) -> eval e2 ((eval e1 env) :: env)
end

(** Now lower Expr.expr to Nameless.expr *)
type cenv = string list

let index (cenv : cenv) (x : string) =
  let rec go (cenv : cenv) (i : int) =
    match cenv with
    | [] -> failwith "not found"
    | head :: tail -> if String.equal head x then i else go tail (i + 1)
  in go cenv 0

let rec lower (expr : Expr.expr) (cenv : cenv) : Nameless.expr =
  match expr with
  | Cst i -> Cst i
  | Add (e1, e2) -> Add ((lower e1 cenv), (lower e2 cenv))
  | Mul (e1, e2) -> Mul ((lower e1 cenv), (lower e2 cenv))
  | Var x -> Var (index cenv x)
  | Let (x, e1, e2) -> Let (lower e1 cenv, lower e2 (x :: cenv))

module Instruction = struct
  type instr = Cst of int | Add | Mul | Var of int | Pop | Swap
  
  let rec eval (instrs : instr list) (stack : int list) =
    match instrs with
    | [] -> (match stack with
            | [x] -> x
            | [] -> failwith "stack is empty"
            | _ -> failwith "stack contains more than 1 element"
            )
    | head :: tail ->
      (match head with
      | Cst i -> eval tail (i :: stack)
      | Add -> (match stack with
               | v1 :: v2 :: tail_ -> eval tail ((v1 + v2) :: tail_)
               | [_] | [] -> failwith "Add: Inadequate candidates for Add")
      | Mul -> (match stack with
                | v1 :: v2 :: tail_ -> eval tail ((v1 * v2) :: tail_)
                | [_] | [] -> failwith "Mul: Inadequate candidates for Mul")
      | Var i -> 
        let rec index stk n =
          (match stk with
          | [] -> failwith "stk is empty"
          | head_ :: tail_ -> if n = 0 then head_ else index tail_(n-1))
        in eval tail ((index stack i) :: stack)
      | Pop ->
        (match stack with
        | [] -> failwith "Pop: empty stack"
        | _ :: tail_ -> eval tail tail_)
      | Swap ->
        (match stack with
         | [] | [_] -> failwith "Swap: Inadequate candidates for Swap"
         | e1 :: e2 :: tail_ -> eval tail (e2 :: e1 :: tail_))
      )
end