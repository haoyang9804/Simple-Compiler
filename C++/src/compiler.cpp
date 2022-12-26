#include <iostream>
#include <unordered_map>
#include <vector>

template <typename Base, typename T> bool isinstanceof(const T *t) {
  return dynamic_cast<const Base *>(t) != nullptr;
}

namespace Expr {
class Expr {
public:
  virtual ~Expr() {}
  virtual std::string expr_name() { return "Expr"; }
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
  Add(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Add"; }
};

class Mul : public Expr {
public:
  Mul() {}
  Mul(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
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
  Let(std::string name, Expr *e1, Expr *e2) : name(name), e1(e1), e2(e2) {}
  std::string name;
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Let"; }
};

typedef std::unordered_map<std::string, int> Env;

int eval(Expr *eptr, Env env) {
  if (isinstanceof<Cst>(eptr)) {
    Cst *cst = static_cast<Cst *>(eptr);
    return cst->val;
  } else if (isinstanceof<Add>(eptr)) {
    Add *add = static_cast<Add *>(eptr);
    return eval(add->e1, env) + eval(add->e2, env);
  } else if (isinstanceof<Mul>(eptr)) {
    Mul *mul = static_cast<Mul *>(eptr);
    return eval(mul->e1, env) * eval(mul->e2, env);
  } else if (isinstanceof<Var>(eptr)) {
    Var *var = static_cast<Var *>(eptr);
    auto pos = env.find(var->name);
    if (pos == env.end()) {
      throw std::logic_error("Cannot find key " + var->name);
    }
    return pos->second;
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    int e1_val = eval(let->e1, env);
    env.insert(std::make_pair(let->name, e1_val));
    return eval(let->e2, env);
  } else {
    throw std::logic_error("Unsupported expr in Expr::eval: " + eptr->expr_name());
  }
}

std::string to_str(Expr *eptr) {
  std::string str = "";
  if (isinstanceof<Cst>(eptr)) {
    Cst *cst = static_cast<Cst *>(eptr);
    str += std::to_string(cst->val);
  } else if (isinstanceof<Add>(eptr)) {
    Add *add = static_cast<Add *>(eptr);
    str += to_str(add->e1) + " + " + to_str(add->e2);
  } else if (isinstanceof<Mul>(eptr)) {
    Mul *mul = static_cast<Mul *>(eptr);
    str += to_str(mul->e1) + " * " + to_str(mul->e2);
  } else if (isinstanceof<Var>(eptr)) {
    Var *var = static_cast<Var *>(eptr);
    str += var->name;
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    str +=
        "let " + let->name + " = " + to_str(let->e1) + " in " + to_str(let->e2);
  } else {
    throw std::logic_error("Unsupported expr in Expr::to_str: " + eptr->expr_name());
  }
  return str;
}

} // namespace Expr

namespace Nameless {
class Expr {
public:
  virtual ~Expr() {}
  virtual std::string expr_name() { return "Expr"; }
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
  Add(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Add"; }
};

class Mul : public Expr {
public:
  Mul() {}
  Mul(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Mul"; }
};

class Var : public Expr {
public:
  Var() {}
  Var(int index) : index(index) {}
  int index;
  std::string expr_name() { return "Var"; }
};

class Let : public Expr {
public:
  Let() {}
  Let(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Let"; }
};

typedef std::vector<int> Env;

int eval(Expr* eptr, Env env) {
  if (isinstanceof<Cst>(eptr)) {
    Cst *cst = static_cast<Cst *>(eptr);
    return cst->val;
  } else if (isinstanceof<Add>(eptr)) {
    Add *add = static_cast<Add *>(eptr);
    return eval(add->e1, env) + eval(add->e2, env);
  } else if (isinstanceof<Mul>(eptr)) {
    Mul *mul = static_cast<Mul *>(eptr);
    return eval(mul->e1, env) * eval(mul->e2, env);
  } else if (isinstanceof<Var>(eptr)) {
    Var *var = static_cast<Var *>(eptr);
    if (env.size() <= var->index) {
      throw std::logic_error("var's index is out of env's scope");
    }
    return env[var->index];
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    int e1_val = eval(let->e1, env);
    env.push_back(e1_val);
    return eval(let->e2, env);
  } else {
    throw std::logic_error("Unsupported expr in Nameless::eval: " + eptr->expr_name());
  }
}

std::string to_str(Expr* eptr) {
  std::string str = "";
  if (isinstanceof<Cst>(eptr)) {
    Cst *cst = static_cast<Cst *>(eptr);
    str += std::to_string(cst->val);
  } else if (isinstanceof<Add>(eptr)) {
    Add *add = static_cast<Add *>(eptr);
    str += to_str(add->e1) + " + " + to_str(add->e2);
  } else if (isinstanceof<Mul>(eptr)) {
    Mul *mul = static_cast<Mul *>(eptr);
    str += to_str(mul->e1) + " * " + to_str(mul->e2);
  } else if (isinstanceof<Var>(eptr)) {
    Var *var = static_cast<Var *>(eptr);
    str += "Var(" + std::to_string(var->index) + ")";
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    str +=
        "let " + to_str(let->e1) + " in " + to_str(let->e2);
  } else {
    throw std::logic_error("Unsupported expr in Expr::to_str: " + eptr->expr_name());
  }
  return str;
}

}

namespace Compiler {

typedef std::vector<std::string> CEnv;

int findIndex(CEnv cenv, std::string name) {
  int index = 0;
  for(std::string str : cenv) {
    if (str == name) {
      return index;
    }
    index ++;
  }
  throw std::logic_error("Cannot find name " + name + " in cenv");
}

Nameless::Expr* lowerFromExprToNameless(Expr::Expr* eptr, CEnv cenv) {
  if (isinstanceof<Expr::Cst>(eptr)) {
    Expr::Cst* cst = static_cast<Expr::Cst*>(eptr);
    return new Nameless::Cst(cst->val);
  }
  else if(isinstanceof<Expr::Add>(eptr)) {
    Expr::Add* add = static_cast<Expr::Add*>(eptr);
    return new Nameless::Add(lowerFromExprToNameless(add->e1, cenv), lowerFromExprToNameless(add->e2, cenv));
  }
  else if(isinstanceof<Expr::Mul>(eptr)) {
    Expr::Mul* mul = static_cast<Expr::Mul*>(eptr);
    return new Nameless::Mul(lowerFromExprToNameless(mul->e1, cenv), lowerFromExprToNameless(mul->e2, cenv));
  }
  else if(isinstanceof<Expr::Var>(eptr)) {
    Expr::Var* var = static_cast<Expr::Var*>(eptr);
    return new Nameless::Var(findIndex(cenv, var->name));
  }
  else if(isinstanceof<Expr::Let>(eptr)) {
    Expr::Let* let = static_cast<Expr::Let*>(eptr);
    cenv.push_back(let->name);
    return new Nameless::Let(lowerFromExprToNameless(let->e1, cenv), lowerFromExprToNameless(let->e2, cenv));
  }
  else {
    throw std::logic_error("Unsupported expr in Nameless::eval: " + eptr->expr_name());
  } 
}

}