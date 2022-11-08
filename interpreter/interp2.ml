open Core

module Expr = struct
  type expr =
    | Cst of int
    | Add of expr * expr
    | Mul of expr * expr
    | Var of string
    | Let of string * expr * expr
    | Fn of string list * expr
    | App of expr * expr list

  type value =
  | Vint of int
  | Vclosure of env * (string list) * expr
  and env = (string, value, String.comparator_witness) Map.t
  
  let empty_env = Map.empty (module String)

  let update_env (env:env) (k:string) (v:value) : env =
    Map.add_exn env ~key:k ~data:v

  let vadd v1 v2 =
    match (v1, v2) with
    | Vint i1, Vint i2 -> Vint (i1 + i2)
    | _, _ -> failwith "Unsupported vadd v1 v2"
  
  let vmul v1 v2 =
    match (v1, v2) with
    | Vint i1, Vint i2 -> Vint (i1 * i2)
    | _, _ -> failwith "Unsupported vmul v1 v2"

  let rec bind_params_to_values (params : string list) (values: value list) : env =
    match (params, values) with
    | p :: params_, v :: values_-> 
      let tailenv = bind_params_to_values params_ values_ in
      update_env tailenv p v
    | [], [] -> empty_env
    | _, _ -> failwith "lengths of params and values are not equal"

  let rec eval (expr:expr) (env:env) : value =
    match expr with
    | Cst i -> Vint i
    | Add (e1, e2) -> vadd (eval e1 env) (eval e2 env)
    | Mul (e1, e2) -> vmul (eval e1 env) (eval e2 env)
    | Var s -> Map.find_exn env s
    | Let (s, e1, e2) -> eval e2 (Map.add_exn env ~key:s ~data:(eval e1 env))
    | Fn (strs, e) -> Vclosure (env, strs, e)
    | App (fn, es) -> 
        let Vclosure(env_closure, params, body) = eval fn env in
        let vs = List.rev_map es (fun e -> eval e env) in
        let fun_env = 

end
