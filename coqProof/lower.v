(**Only prove a simple language including Const of int, Var of String and Let binding*)
Require Import Coq.Strings.String.
Require Import Coq.FSets.FMapInterface.

Module Expr.
  Inductive expr :=
    | Cst (n: nat)
    | Var (x: string)
    | Let (x: string) (e1 e2: expr).
  
  Definition varMap := 

  Fixpoint eval (e: expr) : nat :=
    match e with
    | Cst n -> n
    | Var
End Expr.
