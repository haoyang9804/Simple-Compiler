#include <iostream>
#include <list>
#include <unordered_map>
#include <variant>
#include <vector>

#define ASSERT(STATEMENT, STR)                                                 \
  if (!(STATEMENT))                                                            \
  throw std::logic_error(STR)

#define ALARM(STR) throw std::logic_error(STR)

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
  Cst(int val) : val(val) {}
  int val;
  std::string expr_name() { return "Cst"; }
};

class Add : public Expr {
public:
  Add(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Add"; }
};

class Mul : public Expr {
public:
  Mul(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Mul"; }
};

class Var : public Expr {
public:
  Var(std::string name) : name(name) {}
  std::string name;
  std::string expr_name() { return "Var"; }
};

class Let : public Expr {
public:
  Let(std::string name, Expr *e1, Expr *e2) : name(name), e1(e1), e2(e2) {}
  std::string name;
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Let"; }
};

class Fn : public Expr {
public:
  Fn(const std::vector<std::string> &params, Expr *expr)
      : params(params), expr(expr) {}
  Fn(std::vector<std::string> &&params, Expr *expr)
      : params(std::move(params)), expr(expr) {}
  std::vector<std::string> params;
  Expr *expr;
};

using STRING_OR_EXPR = std::variant<std::string, Expr *>;
bool is_string(STRING_OR_EXPR soe) {
  try {
    std::get<std::string>(soe);
  } catch (const std::bad_variant_access &ex) {
    return false;
  }
  return true;
}

class App : public Expr {
public:
  App(Expr *fn, const std::vector<STRING_OR_EXPR> &arguments)
      : fn(fn), arguments(arguments) {}
  App(Expr *fn, std::vector<STRING_OR_EXPR> &&arguments)
      : fn(fn), arguments(std::move(arguments)) {}
  Expr *fn;
  // argument could be of type string (name of variable)
  // or of Expr, such as Cst(1)
  std::vector<STRING_OR_EXPR> arguments;
};

class Value;
typedef std::unordered_map<std::string, Value *> Env;

class Value {
public:
  virtual ~Value() {}
};

class Vint : public Value {
public:
  Vint(int val) : val(val) {}
  int val;
};

class Vclosure : public Value {
public:
  Vclosure(Env env, const std::vector<std::string> &params, Expr *expr)
      : env(env), params(params), expr(expr) {}
  // Vclosure(Env env, std::vector<std::string> &&params, Expr *expr)
  //     : env(env), params(std::move(params)), expr(expr) {}
  Env env;
  std::vector<std::string> params;
  Expr *expr;
};

Vint *vadd(Value *v1, Value *v2) {
  if (isinstanceof<Vint>(v1) && isinstanceof<Vint>(v2)) {
    Vint *vint1 = static_cast<Vint *>(v1);
    Vint *vint2 = static_cast<Vint *>(v2);
    return new Vint(vint1->val + vint2->val);
  }
  ALARM("vadd type error");
}

Vint *vmul(Value *v1, Value *v2) {
  if (isinstanceof<Vint>(v1) && isinstanceof<Vint>(v2)) {
    Vint *vint1 = static_cast<Vint *>(v1);
    Vint *vint2 = static_cast<Vint *>(v2);
    return new Vint(vint1->val * vint2->val);
  }
  ALARM("vmul type error");
}

Value *eval(Expr *eptr, Env env) {
  if (isinstanceof<Cst>(eptr)) {
    Cst *cst = static_cast<Cst *>(eptr);
    return new Vint(cst->val);
  } else if (isinstanceof<Add>(eptr)) {
    Add *add = static_cast<Add *>(eptr);
    return vadd(eval(add->e1, env), eval(add->e2, env));
  } else if (isinstanceof<Mul>(eptr)) {
    Mul *mul = static_cast<Mul *>(eptr);
    return vmul(eval(mul->e1, env), eval(mul->e2, env));
  } else if (isinstanceof<Var>(eptr)) {
    Var *var = static_cast<Var *>(eptr);
    auto pos = env.find(var->name);
    ASSERT(pos != env.end(), "Cannot find key " + var->name);
    return pos->second;
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    Value *e1_val = eval(let->e1, env);
    env.insert(std::make_pair(let->name, e1_val));
    return eval(let->e2, env);
  } else if (isinstanceof<Fn>(eptr)) {
    Fn *fn = static_cast<Fn *>(eptr);
    return new Vclosure(env, fn->params, fn->expr);
  } else if (isinstanceof<App>(eptr)) {
    App *app = static_cast<App *>(eptr);
    Value *fn_val = eval(app->fn, env);
    ASSERT(isinstanceof<Vclosure>(fn_val),
           "The evaluation result of function is not closure.");
    Vclosure *fn_val_closure = static_cast<Vclosure *>(fn_val);
    Env closure_env = fn_val_closure->env;
    // renew env by assigning parameters the values of arguments
    ASSERT(app->arguments.size() == fn_val_closure->params.size(),
           "arguments' number does not equal to parameters' number");
    size_t arg_size = app->arguments.size();
    for (size_t i = 0; i < arg_size; i++) {
      STRING_OR_EXPR argument = app->arguments[i];
      // argument is a string, representing the name of a variable
      if (is_string(argument)) {
        std::string argument_str = std::get<std::string>(argument);
        Value *arg_val = closure_env.find(argument_str)->second;
        std::string parameter = fn_val_closure->params[i];
        closure_env.insert(std::make_pair(parameter, arg_val));
      }
      // argument is a temporary value, e.g., Add(Cst(1), Cst(2))
      else {
        Expr *argument_expr = std::get<Expr *>(argument);
        Value *arg_val = eval(argument_expr, closure_env);
        std::string parameter = fn_val_closure->params[i];
        closure_env.insert(std::make_pair(parameter, arg_val));
      }
    }
    return eval(fn_val_closure->expr, closure_env);
  } else {
    ALARM("Unsupported expr in Expr::eval: " + eptr->expr_name());
  }
}

// this eval promises to get a int value
int eval_final(Expr *eptr, Env env) {
  Value *value = eval(eptr, env);
  ASSERT(isinstanceof<Vint>(value),
         "Value is not of type Vint in function eval_final");
  Vint *int_value = static_cast<Vint *>(value);
  return int_value->val;
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
  } else if (isinstanceof<Fn>(eptr)) {
    str += "Fn(";
    Fn *fn = static_cast<Fn *>(eptr);
    for (std::string &parameter : fn->params) {
      str += parameter + ", ";
    }
    str = str.substr(0, str.size() - 2);
    str += ")";
    str += "{";
    str += to_str(fn->expr);
    str += "}";
  } else if (isinstanceof<App>(eptr)) {
    App *app = static_cast<App *>(eptr);
    str += to_str(app->fn);
    str += "(";
    for (auto &argument : app->arguments) {
      if (is_string(argument)) {
        str += std::get<std::string>(argument) + ", ";
      } else {
        str += to_str(std::get<Expr *>(argument)) + ", ";
      }
    }
    str = str.substr(0, str.size() - 2);
    str += ")";
  } else {
    ALARM("Unsupported expr in Expr::to_str: " + eptr->expr_name());
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
  Cst(int val) : val(val) {}
  int val;
  std::string expr_name() { return "Cst"; }
};

class Add : public Expr {
public:
  Add(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Add"; }
};

class Mul : public Expr {
public:
  Mul(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Mul"; }
};

class Var : public Expr {
public:
  Var(int index) : index(index) {}
  int index;
  std::string expr_name() { return "Var"; }
};

class Let : public Expr {
public:
  Let(Expr *e1, Expr *e2) : e1(e1), e2(e2) {}
  Expr *e1;
  Expr *e2;
  std::string expr_name() { return "Let"; }
};

class Fn : public Expr {
public:
  Fn(Expr *expr) : expr(expr) {}
  Expr *expr;
};

class App : public Expr {
public:
  App(Expr *expr, const std::vector<Expr *> &arguments)
      : expr(expr), arguments(arguments) {}
  App(Expr *expr, std::vector<Expr *> &&arguments)
      : expr(expr), arguments(std::move(arguments)) {}
  Expr *expr;
  std::vector<Expr *> arguments;
};

class Value;
typedef std::vector<Value *> Env;

class Value {
public:
  virtual ~Value() {}
};

class Vint : public Value {
public:
  Vint(int val) : val(val) {}
  int val;
};

class Vclosure : public Value {
public:
  Vclosure(Env env, Expr *expr) : env(env), expr(expr) {}
  Env env;
  Expr *expr;
};

Vint *vadd(Value *v1, Value *v2) {
  if (isinstanceof<Vint>(v1) && isinstanceof<Vint>(v2)) {
    Vint *vint1 = static_cast<Vint *>(v1);
    Vint *vint2 = static_cast<Vint *>(v2);
    return new Vint(vint1->val + vint2->val);
  }
  ALARM("vadd type error");
}

Vint *vmul(Value *v1, Value *v2) {
  if (isinstanceof<Vint>(v1) && isinstanceof<Vint>(v2)) {
    Vint *vint1 = static_cast<Vint *>(v1);
    Vint *vint2 = static_cast<Vint *>(v2);
    return new Vint(vint1->val * vint2->val);
  }
  ALARM("vmul type error");
}

std::string to_str(Expr *eptr);

Value *eval(Expr *eptr, Env env) {
  if (isinstanceof<Cst>(eptr)) {
    Cst *cst = static_cast<Cst *>(eptr);
    return new Vint(cst->val);
  } else if (isinstanceof<Add>(eptr)) {
    Add *add = static_cast<Add *>(eptr);
    return vadd(eval(add->e1, env), eval(add->e2, env));
  } else if (isinstanceof<Mul>(eptr)) {
    Mul *mul = static_cast<Mul *>(eptr);
    return vmul(eval(mul->e1, env), eval(mul->e2, env));
  } else if (isinstanceof<Var>(eptr)) {
    Var *var = static_cast<Var *>(eptr);
    ASSERT(env.size() > var->index, "var " + std::to_string(var->index) + "'s index is out of env's scope (" + std::to_string(env.size()) + ")");
    return env[var->index];
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    Value *e1_val = eval(let->e1, env);
    env.push_back(e1_val);
    return eval(let->e2, env);
  } else if (isinstanceof<Fn>(eptr)) {
    Fn *fn = static_cast<Fn *>(eptr);
    return new Vclosure(env, fn->expr);
  } else if (isinstanceof<App>(eptr)) {
    App *app = static_cast<App *>(eptr);
    Value *maybe_closure = eval(app->expr, env);
    ASSERT(isinstanceof<Vclosure>(maybe_closure),
           "Expression for application cannot be evaluated into Vclosure");
    Vclosure *closure = static_cast<Vclosure *>(maybe_closure);
    Env closure_env = closure->env;
    for (auto &argument : app->arguments) {
      Value *arg_val = eval(argument, env);
      closure_env.push_back(arg_val);
    }
    // std::cout << "=====" << std::endl;
    // for (int i = 0; i < closure_env.size(); i++) {
    //   if (isinstanceof<Vint>(closure_env[i])) {
    //     Vint* vint = static_cast<Vint*>(closure_env[i]);
    //     std::cout << i << " " << vint->val << std::endl;
    //   }
    //   else {
    //     std::cout << i << std::endl;
    //   }
    // }
    return eval(closure->expr, closure_env);
  } else {
    ALARM("Unsupported expr in Nameless::eval: " + eptr->expr_name());
  }
}

// this eval promises to get a int value
int eval_final(Expr *eptr, Env env) {
  Value *value = eval(eptr, env);
  ASSERT(isinstanceof<Vint>(value),
         "Value is not of type Vint in function eval_final");
  Vint *int_value = static_cast<Vint *>(value);
  return int_value->val;
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
    str += "Var(" + std::to_string(var->index) + ")";
  } else if (isinstanceof<Let>(eptr)) {
    Let *let = static_cast<Let *>(eptr);
    str += "let " + to_str(let->e1) + " in " + to_str(let->e2);
  } else if (isinstanceof<Fn>(eptr)) {
    str += "Fn";
    Fn* fn = static_cast<Fn*>(eptr);
    str += "{";
    str += to_str(fn->expr);
    str += "}";
  } else if (isinstanceof<App>(eptr)) {
    App *app = static_cast<App *>(eptr);
    str += to_str(app->expr);
    str += "(";
    for (auto &argument : app->arguments) {
      str += to_str(argument) + ", ";
    }
    str = str.substr(0, str.size() - 2);
    str += ")";
  }
  else {
    ALARM("Unsupported expr in Nameless::to_str: " + eptr->expr_name());
  }
  return str;
}

} // namespace Nameless

namespace Instruction {
class Instr {
public:
  virtual ~Instr() {}
  virtual std::string expr_name() { return "Instr"; }
};

class Cst : public Instr {
public:
  int val;
  Cst() {}
  Cst(int val) : val(val) {}
  std::string expr_name() { return "Cst"; }
};

class Add : public Instr {
public:
  std::string expr_name() { return "Add"; }
};

class Mul : public Instr {
public:
  std::string expr_name() { return "Mul"; }
};

class Var : public Instr {
public:
  Var() {}
  Var(int index) : index(index) {}
  std::string expr_name() { return "Var"; }
  int index;
};

class Pop : public Instr {
public:
  std::string expr_name() { return "Pop"; }
};

class Swap : public Instr {
public:
  std::string expr_name() { return "Swap"; }
};

typedef std::list<int> Stack;
typedef std::list<Instr *> InstrPtrs;

int eval(InstrPtrs instrs, Stack stack) {
  for (Instr *instrPtr : instrs) {
    if (isinstanceof<Cst>(instrPtr)) {
      Instruction::Cst *cst = static_cast<Instruction::Cst *>(instrPtr);
      stack.insert(stack.begin(), cst->val);
    } else if (isinstanceof<Add>(instrPtr)) {
      ASSERT(stack.size() >= 2,
             "Inadequate values in stack for Add instruction");
      int val1 = stack.front();
      stack.pop_front();
      int val2 = stack.front();
      stack.pop_front();
      stack.insert(stack.begin(), val1 + val2);
    } else if (isinstanceof<Mul>(instrPtr)) {
      ASSERT(stack.size() >= 2,
             "Inadequate values in stack for Mul instruction");
      int val1 = stack.front();
      stack.pop_front();
      int val2 = stack.front();
      stack.pop_front();
      stack.insert(stack.begin(), val1 * val2);
    } else if (isinstanceof<Var>(instrPtr)) {
      Var *varptr = static_cast<Var *>(instrPtr);
      std::list<int>::iterator listi;
      int count = 0;
      for (listi = stack.begin(), count = 0; listi != stack.end();
           ++listi, ++count) {
        if (count == varptr->index) {
          stack.insert(stack.begin(), (*listi));
          break;
        }
      }
    } else if (isinstanceof<Pop>(instrPtr)) {
      stack.pop_front();
    } else if (isinstanceof<Swap>(instrPtr)) {
      int val1 = stack.front();
      stack.pop_front();
      int val2 = stack.front();
      stack.pop_front();
      stack.insert(stack.begin(), val1);
      stack.insert(stack.begin(), val2);
    } else {
      ALARM("Unsupported expr in Instr::to_str: " + instrPtr->expr_name());
    }
  }
  ASSERT(stack.size() == 1,
         "Incorrect number of elements in stack, and size equals " +
             std::to_string(stack.size()));
  return *(stack.begin());
}

std::string to_str(InstrPtrs instrs) {
  std::string str = "";
  for (Instr *instr : instrs) {
    if (isinstanceof<Cst>(instr)) {
      Cst *cstptr = static_cast<Cst *>(instr);
      str += ">| Cst " + std::to_string(cstptr->val) + "\n";
    } else if (isinstanceof<Add>(instr)) {
      str += ">| Add\n";
    } else if (isinstanceof<Mul>(instr)) {
      str += ">| Mul\n";
    } else if (isinstanceof<Var>(instr)) {
      Var *varptr = static_cast<Var *>(instr);
      str += ">| Var" + std::to_string(varptr->index) + "\n";
    } else if (isinstanceof<Pop>(instr)) {
      str += ">| Pop\n";
    } else if (isinstanceof<Swap>(instr)) {
      str += ">| Swap\n";
    } else {
      ALARM("Unsupported instr in Instr::to_str: " + instr->expr_name());
    }
  }
  return str;
}

} // namespace Instruction

namespace Compiler {

typedef std::vector<std::string> CEnv;

int findIndex(CEnv cenv, std::string name) {
  int index = 0;
  for (std::string str : cenv) {
    if (str == name) {
      return index;
    }
    index++;
  }
  ALARM("Cannot find name " + name + " in cenv");
}

// lastcenv is for handling the following situation:
// If we have an app expr `fn(x y){x + y}(1 2)`, then after
// we know the cenv of fn, we immediately use it for 
CEnv lastcenv = {};

Nameless::Expr *lowerFromExprToNameless(Expr::Expr *eptr, CEnv cenv) {
  // std::cout << Expr::to_str(eptr) << std::endl;
  if (isinstanceof<Expr::Cst>(eptr)) {
    Expr::Cst *cst = static_cast<Expr::Cst *>(eptr);
    return new Nameless::Cst(cst->val);
  } else if (isinstanceof<Expr::Add>(eptr)) {
    Expr::Add *add = static_cast<Expr::Add *>(eptr);
    return new Nameless::Add(lowerFromExprToNameless(add->e1, cenv),
                             lowerFromExprToNameless(add->e2, cenv));
  } else if (isinstanceof<Expr::Mul>(eptr)) {
    Expr::Mul *mul = static_cast<Expr::Mul *>(eptr);
    return new Nameless::Mul(lowerFromExprToNameless(mul->e1, cenv),
                             lowerFromExprToNameless(mul->e2, cenv));
  } else if (isinstanceof<Expr::Var>(eptr)) {
    Expr::Var *var = static_cast<Expr::Var *>(eptr);
    return new Nameless::Var(findIndex(cenv, var->name));
  } else if (isinstanceof<Expr::Let>(eptr)) {
    Expr::Let *let = static_cast<Expr::Let *>(eptr);
    Nameless::Expr* ne1 = lowerFromExprToNameless(let->e1, cenv);
    cenv.push_back(let->name);
    return new Nameless::Let(ne1,
                             lowerFromExprToNameless(let->e2, cenv));
  } else if (isinstanceof<Expr::Fn>(eptr)) {
    Expr::Fn *fn = static_cast<Expr::Fn *>(eptr);
    for(auto& param : fn->params) {
      cenv.push_back(param);
    }
    Nameless::Expr *body = lowerFromExprToNameless(fn->expr, cenv);
    return new Nameless::Fn(body);
  } else if (isinstanceof<Expr::App>(eptr)) {
    Expr::App *app = static_cast<Expr::App *>(eptr);
    Nameless::Expr* fn = lowerFromExprToNameless(app->fn, cenv);
    std::vector<Nameless::Expr *> nameless_arguments = {};
    for (Expr::STRING_OR_EXPR argument : app->arguments) {
      if(Expr::is_string(argument)) {
        nameless_arguments.push_back(new Nameless::Var(findIndex(cenv, std::get<std::string>(argument))));
      }
      else {
        nameless_arguments.push_back(lowerFromExprToNameless(std::get<Expr::Expr*>(argument), cenv));
      }
    }
    return new Nameless::App(fn, std::move(nameless_arguments));
  }
  else {
    ALARM("Unsupported expr in Nameless::eval: " + eptr->expr_name());
  }
}

class AbstractVal {
public:
  virtual ~AbstractVal() {}
};

class Slocal : public AbstractVal {};

class Stmp : public AbstractVal {};

typedef std::list<AbstractVal *> AEnv;

int findIndexofInstructionVar(int NamelessVarIndex, AEnv aenv) {
  int index = -1;
  size_t aenv_siz = aenv.size();
  int count;
  auto li = aenv.rbegin();
  for (count = 0; li != aenv.rend(); ++li, ++count) {
    if (isinstanceof<Slocal>(*(li))) {
      index++;
    } else if (isinstanceof<Stmp>(*(li))) {
      continue;
    } else {
      ALARM("Unsupported AbstractVar in findIndexofInstructionVar");
    }
    if (index == NamelessVarIndex) {
      return aenv_siz - 1 - count;
    }
  }
  ALARM("Cannot find NamelessVarIndex " + std::to_string(NamelessVarIndex) +
        " in aenv");
}

Instruction::InstrPtrs lowerFromNamelessToInstruction(Nameless::Expr *eptr,
                                                      AEnv aenv) {
  if (isinstanceof<Nameless::Cst>(eptr)) {
    Nameless::Cst *cst = static_cast<Nameless::Cst *>(eptr);
    return {new Instruction::Cst(cst->val)};
  } else if (isinstanceof<Nameless::Add>(eptr)) {
    Nameless::Add *add = static_cast<Nameless::Add *>(eptr);
    auto e1 = add->e1;
    auto e2 = add->e2;
    Instruction::InstrPtrs instrPtrs = lowerFromNamelessToInstruction(e1, aenv);
    aenv.push_front(new Stmp());
    instrPtrs.splice(instrPtrs.end(), lowerFromNamelessToInstruction(e2, aenv));
    instrPtrs.push_back(new Instruction::Add());
    return instrPtrs;
  } else if (isinstanceof<Nameless::Mul>(eptr)) {
    Nameless::Mul *mul = static_cast<Nameless::Mul *>(eptr);
    auto e1 = mul->e1;
    auto e2 = mul->e2;
    Instruction::InstrPtrs instrPtrs = lowerFromNamelessToInstruction(e1, aenv);
    auto it = instrPtrs.end();
    aenv.push_front(new Stmp());
    instrPtrs.splice(it, lowerFromNamelessToInstruction(e2, aenv));
    instrPtrs.push_back(new Instruction::Mul());
    return instrPtrs;
  } else if (isinstanceof<Nameless::Var>(eptr)) {
    Nameless::Var *var = static_cast<Nameless::Var *>(eptr);
    int index = findIndexofInstructionVar(var->index, aenv);
    return {new Instruction::Var(index)};
  } else if (isinstanceof<Nameless::Let>(eptr)) {
    Nameless::Let *let = static_cast<Nameless::Let *>(eptr);
    auto e1 = let->e1;
    auto e2 = let->e2;
    Instruction::InstrPtrs instrPtrs = lowerFromNamelessToInstruction(e1, aenv);
    aenv.push_front(new Slocal());
    Instruction::InstrPtrs instrPtrs2 =
        lowerFromNamelessToInstruction(e2, aenv);
    instrPtrs.splice(instrPtrs.end(), instrPtrs2);
    instrPtrs.push_back(new Instruction::Swap());
    instrPtrs.push_back(new Instruction::Pop());
    return instrPtrs;
  }
  ALARM("Unsupported Nameless::Expr in lowerFromNamelessToInstruction: " +
        eptr->expr_name());
}

} // namespace Compiler