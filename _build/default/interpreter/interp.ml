type expr = 
  | Cst of int
  | Add of expr * expr
  | Mul of expr * expr