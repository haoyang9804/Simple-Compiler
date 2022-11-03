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