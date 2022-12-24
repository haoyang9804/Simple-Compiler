#include <iostream>
#include <vector>
#include <map>

template<typename Base, typename T>
bool isinstanceof(const T* t) {
  return dynamic_cast<const Base*>(t) != nullptr;
}

namespace Expr {
class Expr {
public:
  virtual ~Expr() {}
  virtual std::string expr_name() {
    return "Expr";
  }
};

class Cst : public Expr {
public:
  Cst() {}
  Cst(int val) : val(val) {}
  int val;
  std::string expr_name() { return "Cst"; }
};

class Add : public Expr {
public:
  Add() {}
  Add(Expr* e1, Expr* e2) : e1(e1), e2(e2) {}
  Expr* e1;
  Expr* e2;
  std::string expr_name() { return "Add"; }
};

class Mul : public Expr {
public:
  Mul() {}
  Mul(Expr* e1, Expr* e2) : e1(e1), e2(e2) {}
  Expr* e1;
  Expr* e2;
  std::string expr_name() { return "Mul"; }
};

class Var : public Expr {
public:
  Var() {}
  Var(std::string name) : name(name) {}
  std::string name;
  std::string expr_name() { return "Var"; }
};

class Let : public Expr {
public:
  Let() {}
  Let(std::string name, Expr* e1, Expr* e2) : name(name), e1(e1), e2(e2) {}
  std::string name;
  Expr* e1;
  Expr* e2;
  std::string expr_name() { return "Let"; }
};

typedef std::map<std::string, int> Env;

int eval(Expr* eptr, Env env) {
  if(isinstanceof<Cst>(eptr)) {
    Cst* cst = static_cast<Cst*>(eptr);
    return cst->val;
  }
  else if(isinstanceof<Add>(eptr)) {
    Add* add = static_cast<Add*>(eptr);
    return eval(add->e1, env) + eval(add->e2, env);
  }
  else if(isinstanceof<Mul>(eptr)) {
    Mul* mul = static_cast<Mul*>(eptr);
    return eval(mul->e1, env) * eval(mul->e2, env);
  }
  else if(isinstanceof<Var>(eptr)) {
    Var* var = static_cast<Var*>(eptr);
    auto pos = env.find(var->name);
    if (pos == env.end()) {
      throw std::logic_error("Cannot find key " + var->name);
    }
    return pos->second;
  }
  else if(isinstanceof<Let>(eptr)) {
    Let* let = static_cast<Let*>(eptr);
    int e1_val = eval(let->e1, env);
    env.insert(std::make_pair(let->name, e1_val));
    return eval(let->e2, env);
  }
  else {
    throw std::logic_error("Unsupported expr " + eptr->expr_name());
  }
}

} // namespace Expr
