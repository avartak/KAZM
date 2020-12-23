#include <Parser.h>
#include <Expression.h>
#include <Constant.h>

namespace kazm {

    std::size_t Parser::parseExpList(std::size_t it, const Program& prog, std::vector<std::shared_ptr<Expression> >& ev) throw (Exception) {

        std::size_t n = 0;
        std::size_t m = 0;

        std::shared_ptr<Expression> exp;
        if ( (m = parseExp(it+n, prog, exp)) == 0 || !exp) return 0;
        n += m;
        ev.push_back(exp);
        exp.reset();

        while (true) {

            if (!parseToken(',', it+n)) return n;
            n++;
            if ( (m = parseExp(it+n, prog, exp)) == 0 || !exp) throw Exception(files.back()->filename, tokens[it+n].line, "Expecting a parameter/expression after \',\'");
            n += m;
            ev.push_back(exp);
            exp.reset();

        }

    }

    std::size_t Parser::parseExp(std::size_t it, const Program& prog, std::shared_ptr<Expression>& exp) throw (Exception) {

        std::size_t n = 0;
        std::size_t m = 0;

        m = parseUnary(it+n, prog, exp);
        if (m == 0 || !exp) return 0;
        n += m;

        while (true) {

            std::string op = "";
            if (parseToken('+', it+n) || parseToken('-', it+n) ||
                parseToken('*', it+n) || parseToken('/', it+n) ||
                parseToken('^', it+n))
            {
                op = tokens[it+n].value;
            }

            if (op == "") return n;
            n++;

            std::shared_ptr<Expression> rhs;
            m = parseBinaryRHS(it+n, prog, op, rhs);
            if (m == 0 || !rhs) throw Exception(files.back()->filename, tokens[it+n].line, "Unable to parse expression after " + op);
            exp = std::make_shared<BinaryExpression>(BinaryExpression::GetType(op), std::move(exp), std::move(rhs));
            n += m;

        }

    } 

    std::size_t Parser::parseBinaryRHS(std::size_t it, const Program& prog, const std::string& preop, std::shared_ptr<Expression>& rhs) throw (Exception) {

        std::size_t n = 0;
        std::size_t m = 0;

        std::shared_ptr<Expression> r1;
        m = parseUnary(it+n, prog, r1);
        if (m == 0 || !r1) return 0;
        n += m;

        while (true) {

            std::string op = "";
            if (parseToken('+', it+n) || parseToken('-', it+n) ||
                parseToken('*', it+n) || parseToken('/', it+n) ||
                parseToken('^', it+n))
            {
                op = tokens[it+n].value;
            }

            if (op == "" || BinaryExpression::GetPrecedence(op) < BinaryExpression::GetPrecedence(preop)) {
                rhs = r1;
                if (op != "") return n+1;
                else return n;
            }
            n++;

            std::shared_ptr<Expression> r2;
            m = parseBinaryRHS(it+n, prog, op, r2);
            if (m == 0 || !r2) throw Exception(files.back()->filename, tokens[it+n].line, "Unable to parse expression after " + op);
            r1 = std::make_shared<BinaryExpression>(BinaryExpression::GetType(op), std::move(r1), std::move(r2));
            n += m;            

        }

    }

    std::size_t Parser::parseUnary(std::size_t it, const Program& prog, std::shared_ptr<Expression>& exp) throw (Exception) {

        std::size_t n = 0;
        std::size_t m = 0;

        if (parseToken(T_PI, it) || parseToken(T_REAL, it) || parseToken(T_NNINTEGER, it)) {
            exp = std::make_shared<Constant>(tokens[it].value);
            return 1;
        }

        else if (parseToken(T_ID, it)) {
            std::string pname = tokens[it].value;
            try {
                auto gate = dynamic_cast<const Gate&>(prog);
                auto pmap = gate.param_map;
                if (pmap.find(pname) == pmap.end()) throw Exception(files.back()->filename, tokens[it+n].line, "Unknown parameter " + pname);
                exp = gate.params[pmap[pname]];
            }
            catch (const std::bad_cast& e) {
                throw Exception(files.back()->filename, tokens[it+n].line, "Unknown parameter " + pname);
            }
            return 1;         
        }

        else if (parseToken('+', it) || parseToken('-', it)) {
            char op = ( parseToken('+', it) ? '+' : '-' );
            n++;
            std::shared_ptr<Expression> e;
            m = parseUnary(it+n, prog, e);
            if (m == 0 || !e) throw Exception(files.back()->filename, tokens[it+n].line, "Unable to parse expression after " + tokens[it].value);
            if (op == '+') exp = std::move(e);
            else exp = std::make_shared<UnaryExpression>(unaryop_negate, e);
            return n+m;
        }

        else if (parseToken('(', it)) {
            n++;
            std::shared_ptr<Expression> e;
            m = parseExp(it+n, prog, e);
            if (m == 0 || !e) throw Exception(files.back()->filename, tokens[it+n].line, "Unable to parse expression after \'(\'");
            n += m;
            if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Missing \')\'");
            n++;
            exp = std::move(e);
            return n;
        }

        else if (parseToken(T_SIN, it) || parseToken(T_COS, it) || parseToken(T_TAN, it) || 
                 parseToken(T_EXP, it) || parseToken(T_LN , it) || parseToken(T_SQRT, it))
        {
            std::string unary_str = tokens[it].value;
            n++;
            if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' after " + unary_str);
            n++;
            std::shared_ptr<Expression> e;
            m = parseExp(it+n, prog, e);
            if (m == 0 || !e) throw Exception(files.back()->filename, tokens[it+n].line, "Unable to parse expression after \'(\'");
            n += m;
            if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Missing \')\'");
            n++;
            exp = std::make_shared<UnaryExpression>(UnaryExpression::GetType(unary_str), e);
            return n;
        }

        return n;

    }

}
