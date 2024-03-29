#include "Statement.h"
#include "Utils/StreamMethods.h"

namespace Jam::Eq
{

    OStream& operator<<(OStream& out, const StackValue& v)
    {
        out << "{ ";
        out << "c:" << Hex(v.c);
        out << ", ";
        out << "f:" << Hex(v.f);
        out << ", ";
        out << "v:" << SetD({v.v}, sizeof(double) << 1, 14);
        out << " }";
        return out;
    }

    void trace(EvalStack&    stack,
               const String& message,
               const bool    topToBottom = true)
    {
        const int iOffs = stack.sizeI();

        Dbg::println(Tab(4), message);

        for (int i = 0; i < iOffs; ++i)
        {
            int c = iOffs - i - 1;
            if (!topToBottom)
                c = i;
            Dbg::println(Tab(4), SetI({i}), ':', ' ', stack[c]);
        }
    }

    void Statement::push(const R64& v, const size_t& idx, const U8 flag)
    {
        _stack.push({v, idx, flag});
    }

    void Statement::push(const Symbol* sy)
    {
        push(sy->value());
    }

    void Statement::store(const Symbol* sym)
    {
        size_t idx = _variables.find(sym->name());
        if (idx == JtNpos)
        {
            _variables.insert(sym->name(), {sym->value()});
            idx = _variables.find(sym->name());
        }
        push(_variables.at(idx).v, idx, StackValue::Id);
    }

    void Statement::add()
    {
        if (_stack.size() > 1)
        {
            const R64& b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            push(a + b);
        }
        else
            argError("add");
    }

    void Statement::sub()
    {
        if (_stack.size() > 1)
        {
            const R64& b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            push(a - b);
        }
        else
            argError("sub");
    }

    void Statement::neg()
    {
        if (_stack.isNotEmpty())
        {
            const R64& a = _stack.popTop().v;
            push(-a);
        }
        else
            argError("neg");
    }

    void Statement::mul()
    {
        if (_stack.size() > 1)
        {
            const R64& b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            push(a * b);
        }
        else
            argError("mul");
    }

    void Statement::div()
    {
        if (_stack.size() > 1)
        {
            R64        b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            if (abs(b) > DBL_EPSILON)
                b = 1.0 / b;
            else
                b = NAN;

            push(a * b);
        }
        else
            argError("div");
    }

    void Statement::mod()
    {
        if (_stack.size() > 1)
        {
            const R64& b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            push(R64(fmod(a, b)));
        }
        else
            argError("mod");
    }

    void Statement::pow()
    {
        if (_stack.size() > 1)
        {
            const R64& b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            push(::pow(a, b));
        }
        else
            argError("pow");
    }

    void Statement::group()
    {
        if (_stack.size() > 1)
        {
            const auto& [v, c, f] = _stack.popTop();
            if (const U8 nr = U8(v);
                _stack.size() > nr && nr > 0)
            {
                ValueGrouping* vg = new ValueGrouping();
                for (U8 i = 0; i < nr; ++i)
                    vg->push_back(_stack.popTop());

                _groups.insert(_hashCount, vg);

                _stack.push({
                    R64(_hashCount),
                    _hashCount,
                    StackValue::List,
                });

                _hashCount++;
            }
        }
        else
            argError("group");
    }

    void Statement::assign()
    {
        if (_stack.size() > 1)
        {
            StackValue       c{NAN, JtNpos, 0};
            const StackValue b = _stack.popTop();
            const StackValue a = _stack.popTop();

            // a = b
            if (b.isId())
            {
                if (b.c < _variables.size())
                {
                    c.v = _variables[b.c].v;
                    c.c = b.c;
                    c.f = StackValue::Id;
                }
            }
            else if (b.isList())
            {
                c.v = R64(b.c);
                c.c = b.c;
                c.f = StackValue::List;
            }
            else
            {
                c.v = b.v;
                c.c = JtNpos;
                c.f = StackValue::Value;
            }

            if (a.isId())
            {
                if (a.c < _variables.size())
                    _variables[a.c] = c;
            }

            _stack.push(c);
        }
        else
            argError("assign");
    }

    void Statement::mathFncA1(WrapFuncA1 f)
    {
        if (_stack.size() > 1)
        {
            if (_stack.popTop().integer() != 1)
            {
                error(
                    "expected one argument to the "
                    "supplied math function");
            }
            const R64& a = _stack.popTop().v;
            push(f(a));
        }
        else
            error(
                "supplied math function requires "
                "at least two elements on the stack.");
    }

    void Statement::mathFncA2(WrapFuncA2 f)
    {
        if (_stack.size() > 2)
        {
            if (_stack.popTop().integer() != 2)
            {
                error(
                    "expected two argument to the "
                    "supplied math function");
            }
            const R64& b = _stack.popTop().v;
            const R64& a = _stack.popTop().v;
            push(f(a, b));
        }
        else
            error(
                "supplied math function requires "
                "at least three elements on the stack.");
    }

    double lMod(const double a, const double b)
    {
        const double r = remainder(a, b);
        return r < 0 ? b + r : r;
    }

    void Statement::eval(const Symbol* sy)
    {
        // clang-format off
    switch (sy->type()) {
    case Numerical  : push(sy);         break;
    case Identifier : store(sy);        break;
    case MathPi     : push(Pi64);       break;
    case MathE      : push(E64);        break;
    case Add        : add();            break;
    case Sub        : sub();            break;
    case Neg        : neg();            break;
    case Mul        : mul();            break;
    case Div        : div();            break;
    case Pow        : pow();            break;
    case Mod        : mod();            break;
    case Assignment : assign();         break;
    case Grouping   : group();          break;
    case MathSin    : mathFncA1(sin);   break;
    case MathAtan   : mathFncA1(atan);  break;
    case MathAbs    : mathFncA1(fabs);  break;
    case MathAcos   : mathFncA1(acos);  break;
    case MathAsin   : mathFncA1(asin);  break;
    case MathAtan2  : mathFncA2(atan2); break;
    case MathCeil   : mathFncA1(ceil);  break;
    case MathCos    : mathFncA1(cos);   break;
    case MathCosh   : mathFncA1(cosh);  break;
    case MathExp    : mathFncA1(exp);   break;
    case MathFabs   : mathFncA1(fabs);  break;
    case MathFloor  : mathFncA1(floor); break;
    case MathFmod   : mathFncA2(lMod);  break;
    case MathLog    : mathFncA1(log);   break;
    case MathLog10  : mathFncA1(log10); break;
    case MathPow    : mathFncA2(::pow); break;
    case MathSinh   : mathFncA1(sinh);  break;
    case MathSqrt   : mathFncA1(sqrt);  break;
    case MathTan    : mathFncA1(tan);   break;
    case MathTanh   : mathFncA1(tanh);  break;
    case UserFunction: 
    case None:
    case Not:
    case BitwiseNot:
    default:
        break;
    }
        // clang-format on
    }

    R64 Statement::executeImpl(const SymbolArray& val)
    {
        _stack.resizeFast(0);
        for (const auto& sy : val)
            eval(sy);
        // trace(_stack, "RESULTS");
        return _stack.empty() ? 0 : _stack.top().v;
    }

    void Statement::argError(const char* op)
    {
        error(
            "not enough arguments "
            "supplied to the '",
            op,
            "' operation ");
    }

    R64 Statement::execute(const SymbolArray& val)
    {
        try
        {
            return executeImpl(val);
        }
        catch (...)
        {
            _stack.resizeFast(0);
            return 0;
        }
    }

    void Statement::set(const String& name, const R64 value)
    {
        if (const size_t idx = _variables.find(name);
            idx == JtNpos)
            _variables.insert(name, {value, JtNpos, StackValue::Value});
        else
            _variables[idx] = {value, JtNpos, StackValue::Value};
    }

    void Statement::set(const VInt index, const R64 value)
    {
        if (index < _variables.size())
            _variables[index] = {value, JtNpos, StackValue::Value};
    }

    VInt Statement::indexOf(const String& name) const
    {
        return _variables.find(name);
    }

    R64 Statement::get(const String& name, const R64 def)
    {
        if (const size_t idx = _variables.find(name);
            idx != JtNpos)
            return _variables[idx].v;
        return def;
    }

    R64 Statement::get(const VInt name, const R64 def)
    {
        if (name < _variables.size())
            return _variables[name].v;
        return def;
    }

    R64 Statement::peek(I32 idx)
    {
        idx = (_stack.topI() - idx);
        if (idx >= 0 && idx < _stack.sizeI())
            return _stack[idx].v;
        return 0;
    }

    void Statement::get(const String& name, ValueList& dest)
    {
        dest.resizeFast(0);
        const U32 hashKey = (U32)get(name, -1);

        if (const size_t idx = _groups.find(hashKey);
            idx != JtNpos)
        {
            if (ValueGrouping* vg = _groups.at(idx);
                vg != nullptr)
            {
                for (I32 i = vg->sizeI() - 1; i >= 0; --i)
                    dest.push_back(vg->at(i).v);
            }
        }
    }

    Statement::~Statement()
    {
        for (const auto& ele : _groups)
            delete ele.second;
    }

}  // namespace Jam::Eq
