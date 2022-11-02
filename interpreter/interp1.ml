open Core

module Expr = struct
  type expr =
    | Cst of int
    | Add of expr * expr
    | Mul of expr * expr
    | Var of string
    | Let of string * expr * expr

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
