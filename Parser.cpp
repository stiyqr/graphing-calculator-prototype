#include <iostream>
#include <deque>
#include <map>
#include <cmath>
#include <QDebug>

#include "Parser.h"

Parser::Parser(QString str) {
    // remove white space from input
    inputVar = "";
    inputStr = str;
    inputStr = inputStr.simplified();
    inputStr.replace(" ", "");

    resultToken.setValue("");

    // read input before '=' a
    parseInputVar();

    // read input after '='
    if (inputType == Parser::IType::X || inputType == Parser::IType::Y) {

        if (inputStr.length() == 3) {
            inputStr += "+0";
        }

        qDebug() << "ABOUT TO PARSE MAIN STRING";
        parseMainStr();
        readMainStack();
    }
    else if (inputType == Parser::IType::VAR) {
        qDebug() << "ABOUT TO SAVE VARIABLE";
        checkVarInputValidity();
    }
    else if (inputType == Parser::IType::ERR) {
        inputValid = false;
        qDebug() << "input type error, cannot parse main string";
        outputStr = "Error: input invalid";
    }
}

void Parser::init(QString str) {
    qDebug() << "initialize parser";

    *this = {};
    //qDebug() << "INIT x: " << var.variables["x"];

    // remove white space from input
    inputVar = "";
    inputStr = str;
    inputStr = inputStr.simplified();
    inputStr.replace(" ", "");

    resultToken.setValue("");

    // read input before '=' a
    parseInputVar();
    qDebug() << "parseinputvar finished";

    // read input after '='
    if (inputType == Parser::IType::X || inputType == Parser::IType::Y) {
        if (inputStr.length() == 3 || inputStr == "y=-x" || inputStr == "y=+x" || inputStr == "x=-y" || inputStr == "x=+y") {
            inputStr += "+0";
        }

        qDebug() << "ABOUT TO PARSE MAIN STRING";
        parseMainStr();
        readMainStack();
    }
    else if (inputType == Parser::IType::VAR) {
        qDebug() << "ABOUT TO SAVE VARIABLE";
        checkVarInputValidity();
    }
    else {
        inputValid = false;
        qDebug() << "input type error, cannot parse main string";
        outputStr = "Error: input invalid";
    }
}

void Parser::parseInputVar() {
    // insert pi value
    var.variables["pi"] = "3.14159265358979323846";

    // parse input
    while(cursor < inputStr.length()) {
        ////qDebug() << "parsing input var...";
        // skip '='
        if (inputStr[cursor] == '=') {
            cursor++;
            break;
        }

        if (inputStr[cursor] < 'A' || inputStr[cursor] > 'z') {
            inputType = Parser::IType::ERR;
            inputValid = false;
            return;
        }

        if (inputStr[cursor] > 'Z' && inputStr[cursor] < 'a') {
            inputType = Parser::IType::ERR;
            inputValid = false;
            return;
        }

        inputVar += inputStr[cursor];
        cursor++;
    }

    // Error: cannot change value of pi
    if (inputVar == "pi") {
        inputValid = false;
        inputType = Parser::IType::ERR;

        //qDebug() << "cannot change value of pi";
        return;
    }

    // Error: no calculation after '='
    if (cursor == inputStr.length()) {
        inputValid = false;
        inputType = Parser::IType::ERR;

        //qDebug() << "calculation not found";
        return;
    }

    if (inputVar == "x" || inputVar == "X") {
        inputType = Parser::IType::X;
        //qDebug() << "input type x";
    }
    else if (inputVar == "y" || inputVar == "Y") {
        inputType = Parser::IType::Y;
        //qDebug() << "input type y";
    }
    else if (currentToken.isAlphabet(inputVar[0])) {
        //var.variables[inputVar] = "";
        inputType = Parser::IType::VAR;
        //qDebug() << "input type var";
    }
    else {
        inputType = Parser::IType::ERR;
        inputValid = false;
        //qDebug() << "input type err";
    }
}

void Parser::parseMainStr() {
    while(cursor < inputStr.length() && inputValid) {
        qDebug() << "parsing main string...";

        switch(currentToken.getNextToken(inputStr, cursor)) {
        case Token::TType::NUM:
            mainStack.emplace_back(currentToken);

            if (negativeNum) {
                mainStack.emplace_back(Token(double(-1)));
                mainStack.emplace_back(Token('*'));
                negativeNum = false;
            }

            //qDebug() << "push num to mainstack";
            break;

        case Token::TType::VAR:
            mainStack.emplace_back(currentToken);
            //qDebug() << "push var to mainstack";
            break;

        case Token::TType::X:
            mainStack.emplace_back(currentToken);
            //qDebug() << "push x to mainstack";
            //qDebug() << "PUSHING x: " << var.variables["x"];
            break;

        case Token::TType::Y:
            mainStack.emplace_back(currentToken);
            //qDebug() << "push y to mainstack";
            break;

        case Token::TType::OP:
            // precedence lower than last operator
            while (!opStack.empty() && currentToken.getPrecedence() <= opStack.front().getPrecedence()) {
                if (currentToken.getPrecedence() == opStack.front().getPrecedence() && opStack.front().getAssoc() == Token::ASOC::RIGHT)
                    break;

                mainStack.emplace_back(opStack.front());
                opStack.pop_front();
                //qDebug() << "   pop op to mainstack";
            }

            opStack.emplace_front(currentToken);
            //qDebug() << "push op to opstack";
            break;

        case Token::TType::LEFT_BR:
            opStack.emplace_front(currentToken);
            //qDebug() << "push left bracket to opstack";
            break;

        case Token::TType::RIGHT_BR:
            // Error: empty brackets
            if (inputStr[cursor-2] == '(' || inputStr[cursor-2] == '+' || inputStr[cursor-2] == '-' || inputStr[cursor-2] == '*'
                    || inputStr[cursor-2] == '/' || inputStr[cursor-2] == '^') {
                inputValid = false;
                ////qDebug() << "Error: empty brackets";
                break;
            }

            // pop opStack to mainStack until matching parentheses found
            while(opStack.front().getType() != Token::TType::LEFT_BR) {
                if (opStack.empty()) {
                    inputValid = false;
                    ////qDebug() << "Error: no matching left brackets";
                    break;
                }
                else {
                    mainStack.emplace_back(opStack.front());
                    opStack.pop_front();
                    ////qDebug() << "   pop op inside brackets";
                }
            }

            // discard matching parentheses
            if (inputValid){
                opStack.pop_front();
                ////qDebug() << "discard left bracket";

                // pop function beside brackets
                if (opStack.front().getType() == Token::TType::FUNC) {
                    mainStack.emplace_back(opStack.front());
                    opStack.pop_front();
                    ////qDebug() << "   pop function to mainstack";
                }
            }
            break;

        case Token::TType::SIGN_NEG:
            if (negativeNum == false) {
                negativeNum = true;
            }
            else {
                negativeNum = false;
            }
            //qDebug() << "found negative sign";
            break;

        case Token::TType::SIGN_POS:
            // ignore positive sign
            //qDebug() << "found positive sign";
            break;

        case Token::TType::FUNC:
            if (inputStr[cursor] != '(') {
                inputValid = false;
                ////qDebug() << "Error: no brackets after function";
                break;
            }

            opStack.emplace_front(currentToken);
            //qDebug() << "found function";
            break;

        case Token::TType::COMMA:
            // ignore comma
            //qDebug() << "found comma";
            break;

        case Token::TType::ERR:
            inputValid = false;
            //qDebug() << "Error: token type error";
            break;

        default:
            inputValid = false;
            //qDebug() << "Error: default error";
            break;
        }
    }

    // pop remaining opStack to mainStack
    while (!opStack.empty() && inputValid) {
        if (opStack.front().getType() == Token::TType::LEFT_BR) {
            inputValid = false;
            //qDebug() << "Error: no matching right bracket";
            break;
        }

        mainStack.emplace_back(opStack.front());
        opStack.pop_front();
        //qDebug() << "   pop op to mainstack to empty opstack";
    }
}

void Parser::mainStackToStr() {
    outputStr = "";

    for (std::deque<Token>::size_type i = 0; i < mainStack.size(); i++) {
        outputStr += mainStack[i].getValue();
        outputStr += '\n';
        //qDebug() << "RPN: " << mainStack[i].getValue();
    }

    if (inputValid == false) {
        outputStr = "Error: input invalid";
    }
}

void Parser::resultToStr() {
    outputStr = "";

    outputStr = QString::fromStdString(std::to_string(resultToken.getNum()));

    if (inputValid == false) {
        outputStr = "Error: input invalid";
    }
}

QString Parser::getOutputStr() {
    //mainStackToStr();
    if (inputType == Parser::IType::X || inputType == Parser::IType::Y)
        resultToStr();

    return outputStr;
}

long double Parser::readMainStack() {
    // copy main stack to not change the stack
    std::deque<Token> stackCopy = mainStack;

    for (std::deque<Token>::size_type i = 0; i < stackCopy.size() && inputValid && stackCopy.size() > 1; i++) {
        qDebug() << "reading mainstack... inputStr: " << inputStr;

        // found operator in mainstack
        if (stackCopy[i].getType() == Token::TType::OP) {
            ////qDebug() << "SOLVING MAINSTACK: found operator";
            //qDebug() << "solving mainstack x: " << var.variables["x"];

            if (stackCopy.size() < 3) {
                inputValid = false;
                break;
            }

            // pop 2 numbers before op
            Token num1, num2, op, result;
            num1 = stackCopy[i-2];
            num2 = stackCopy[i-1];
            op = stackCopy[i];

            //qDebug() << "values num1: " << num1.getValue() << ", num2: " << num2.getValue() << ", op: " << op.getValue();
            //qDebug() << "numbers num1: " << num1.getNum() << ", num2: " << num2.getNum() << ", op: " << op.getValue();

            // do calculations
            result = calculateOp(num1, num2, op);

            // replace the calculated numbers and operator with result
            stackCopy[i] = result;
            stackCopy.erase(stackCopy.begin() + (i - 2), stackCopy.begin() + i);

            // parse again
            ////qDebug() << "parse again OP";
            i = 0;
            continue;
        }

        // found function in mainstack
        if (stackCopy[i].getType() == Token::TType::FUNC) {
            ////qDebug() << "SOLVING MAINSTACK: found function";
            //qDebug() << "solving mainstack x: " << var.variables["x"];


            Token result;
            if (stackCopy[i].getValue() == "pow") {
                if (stackCopy.size() < 3) {
                    inputValid = false;
                    break;
                }

                Token num1, num2, func;

                // pop two numbers before function
                num1 = stackCopy[i-2];
                //qDebug() << "function x: " << var.variables["x"];
                //qDebug() << "function num1 x num: " << num1.getNum();
                //qDebug() << "function num1 x value: " << num1.getValue();
                num2 = stackCopy[i-1];
                func = stackCopy[i];


                // do calculations
                result = calculateFunc(num1, num2, func);

                // replace the calculated numbers and operator with result
                stackCopy[i] = result;
                stackCopy.erase(stackCopy.begin() + (i - 2), stackCopy.begin() + i);
            }
            else {
                if (stackCopy.size() < 2) {
                    inputValid = false;
                    break;
                }

                Token num, func;

                // pop one number before function
                num = stackCopy[i - 1];
                func = stackCopy[i];

                // do calculations
                result = calculateFunc(num, func);

                // replace the calculated numbers and operator with result
                stackCopy[i] = result;
                stackCopy.erase(stackCopy.begin() + (i - 1), stackCopy.begin() + i);
            }

            // parse again
            ////qDebug() << "parse again FUNC";
            i = 0;
            continue;
        }
    }

    qDebug() << "finished read mainstack loop";

    if (stackCopy[0].getType() == Token::TType::VAR || stackCopy[0].getType() == Token::TType::X || stackCopy[0].getType() == Token::TType::Y) {
        ////qDebug() << "result at [0] is var/x/y";

        // check if variable exist
        auto it = var.variables.find(stackCopy[0].getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            //qDebug() << "variable not found";
        }
        // variable exist, calculate variable value
        else {
            stackCopy[0].setValue(it->second);
            if (stackCopy[0].type == Token::TType::VAR) varParser(stackCopy[0]);
            //qDebug() << "1.2 num1 variable, value: " << stackCopy[0].getValue() << ", number: ", stackCopy[0].getNum();
        }

    }

    ////qDebug() << "stackcopy size = " << stackCopy.size();
    ////qDebug() << "stackcopy[0] num: " << stackCopy[0].getNum() << ", value: " << stackCopy[0].getValue();

    resultToken = stackCopy[0];
    return resultToken.getNum();
}

Token Parser::calculateOp(Token num1, Token num2, Token op) {
    qDebug() << "SOLVING OPERATOR";

    Token result;

    // if num is x or y
    if (num1.getType() == Token::TType::X || num1.getType() == Token::TType::Y) {
        //num1.setValue(QString::number(value));
        auto it = var.variables.find(num1.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        num1.assignNum(it->second.toDouble());
        qDebug() << "assigned num1 xy value";
    }

    if (num2.getType() == Token::TType::X || num2.getType() == Token::TType::Y) {
        //num2.setValue(QString::number(value));
        auto it = var.variables.find(num2.getValue());
        //qDebug() << "IT NAME: " << it->first;
        //qDebug() << "IT VALUE: " << it->second;

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        num2.assignNum(it->second.toDouble());
        qDebug() << "assigned num2 xy value";
    }

    // if num is variable, solve num
    if (num1.getType() == Token::TType::VAR) {
        ////qDebug() << "1. num1 variable, value: " << num1.getValue() << ", number: ", num1.getNum();

        // check if variable exist
        auto it = var.variables.find(num1.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
            //qDebug() << "variable not found";
        }
        else {
            num1.setValue(it->second);
            //qDebug() << "1.2 num1 variable, value: " << num1.getValue() << ", number: ", num1.getNum();
        }

        // variable exist, calculate variable value
        varParser(num1);
        if (!inputValid) return result;
        num1.setValue(num1.getValue());

        ////qDebug() << "2. num1 variable, value: " << num1.getValue() << ", number: ", num1.getNum();
    }

    if (num2.getType() == Token::TType::VAR) {
        ////qDebug() << "1. num2 variable, value: " << num2.getValue() << ", number: ", num2.getNum();

        // check if variable exist
        auto it = var.variables.find(num2.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }
        else {
            num2.setValue(it->second);
        }

        // variable exist, calculate variable value
        varParser(num2);
        if (!inputValid) return result;
        num2.setValue(num2.getValue());

        ////qDebug() << "2. num2 variable, value: " << num2.getValue() << ", number: ", num2.getNum();
    }

    // calculate
    if (op.getValue() == "+") {
        result.assignNum(num1.getNum() + num2.getNum());
    }
    else if (op.getValue() == "-") {
        result.assignNum(num1.getNum() - num2.getNum());
    }
    else if (op.getValue() == "*") {
        result.assignNum(num1.getNum() * num2.getNum());
    }
    else if (op.getValue() == "/") {
        if (num2.getNum() == 0) {
            inputValid = false;
            return result;
        }
        result.assignNum(num1.getNum() / num2.getNum());
    }
    else if (op.getValue() == "^") {
        result.assignNum(pow(num1.getNum(), num2.getNum()));
    }

    //qDebug() << "values num1: " << num1.getValue() << ", num2: " << num2.getValue() << ", op: " << op.getValue();
    //qDebug() << "numbers num1: " << num1.getNum() << ", num2: " << num2.getNum() << ", op: " << op.getValue();
    //qDebug() << "result num: " << result.getNum() << ", value: " << result.getValue();

    return result;
}

Token Parser::calculateFunc(Token num, Token func) {
    qDebug() << "SOLVING FUNCTION";

    Token result;

    // if num is x or y
    if (num.getType() == Token::TType::X || num.getType() == Token::TType::Y) {
        //num.setValue(QString::number(value));
        auto it = var.variables.find(num.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        num.assignNum(it->second.toDouble());
    }

    // if num is variable, solve num
    if (num.getType() == Token::TType::VAR) {
        // check if variable exist
        auto it = var.variables.find(num.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        varParser(num);
        if (!inputValid) return result;
        num.setValue(num.getValue());
    }

    // calculate
    if (func.getValue() == "sin") {
        result.assignNum(std::sin(num.getNum()));
    }
    else if (func.getValue() == "cos") {
        result.assignNum(std::cos(num.getNum()));
    }
    else if (func.getValue() == "tan") {
        result.assignNum(std::tan(num.getNum()));
    }
    else if (func.getValue() == "sqrt") {
        result.assignNum(std::sqrt(num.getNum()));
    }

    return result;
}

Token Parser::calculateFunc(Token num1, Token num2, Token func) {
    qDebug() << "SOLVING FUNCTION SQRT";

    Token result;

    // if num is x or y
    if (num1.getType() == Token::TType::X || num2.getType() == Token::TType::Y) {
        //num1.setValue(QString::number(value));
        auto it = var.variables.find(num1.getValue());
        ////qDebug() << "IT NAME: " << it->first;
        ////qDebug() << "IT VALUE: " << it->second;

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        num1.assignNum(it->second.toDouble());
        //qDebug() << "CALCULATE FUNC num1: " << num1.getNum();
    }

    if (num2.getType() == Token::TType::X || num2.getType() == Token::TType::Y) {
        //num2.setValue(QString::number(value));
        auto it = var.variables.find(num2.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        num2.assignNum(it->second.toDouble());
        //qDebug() << "CALCULATE FUNC num2: " << num2.getNum();
    }

    // if num is variable, solve num
    if (num1.getType() == Token::TType::VAR) {
        // check if variable exist
        auto it = var.variables.find(num1.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        varParser(num1);
        if (!inputValid) return result;
        num1.setValue(num1.getValue());
    }

    if (num2.getType() == Token::TType::VAR) {
        // check if variable exist
        auto it = var.variables.find(num2.getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            return result;
        }

        // variable exist, calculate variable value
        varParser(num2);
        if (!inputValid) return result;
        num2.setValue(num2.getValue());
    }

    // calculate
    if (func.getValue() == "pow") {
        result.assignNum(std::pow(num1.getNum(), num2.getNum()));
    }

    return result;
}

void Parser::checkVarInputValidity() {
    qDebug() << "checkvarinputvalidity";

    int varCursor = cursor;
    parseMainStr();
    readMainStack();
    if (inputValid) {
        var.variables[inputVar] = QString::fromStdString(inputStr.toStdString().substr(varCursor));
        //qDebug() << "variable input valid";
        //qDebug() << "variable input: " << var.variables[inputVar];
        //qDebug() << "string input: " << QString::fromStdString(inputStr.toStdString().substr(varCursor));
        //qDebug() << "string input: " << inputStr;
        //qDebug() << "cursor pos: " << varCursor;
    }
    else {
        outputStr = "Error: Variable input invalid";
        //qDebug() << "variable input invalid";
    }
}

void Parser::varParser(Token& varToken) {
    std::deque<Token>varMainstack, varOpstack;
    QString varStr = varToken.getValue();
    int varCursor = 0;

    if (varStr == "") {
        inputValid = false;
        return;
    }

    qDebug() << "VARPARSER: varstr: " << varStr << "varCursor: " << varCursor;

    while(varCursor < varStr.length() && inputValid) {
        qDebug() << "while varparser...";
        ////qDebug() << "varCursor = " << varCursor;
        switch(currentToken.getNextToken(varStr, varCursor)) {
        case Token::TType::NUM:
            varMainstack.emplace_back(currentToken);

            if (negativeNum) {
                varMainstack.emplace_back(Token(double(-1)));
                varMainstack.emplace_back(Token('*'));
                negativeNum = false;
            }

            ////qDebug() << "push num to varMainstack";
            break;

        case Token::TType::VAR:
            // check for variable loop
            if (currentToken.getValue() == inputVar) {
                inputValid = false;
                ////qDebug() << "Error: token type error, variable loop";
                ////qDebug() << "varStr: " << varStr;
                ////qDebug() << "token value: " << currentToken.getValue();
                break;
            }

            varMainstack.emplace_back(currentToken);
            //qDebug() << "push var to varMainstack";
            break;

        case Token::TType::X:
            varMainstack.emplace_back(currentToken);
            //qDebug() << "push x to varMainstack";
            break;

        case Token::TType::Y:
            varMainstack.emplace_back(currentToken);
            //qDebug() << "push y to varMainstack";
            break;

        case Token::TType::OP:
            // precedence lower than last operator
            while (!varOpstack.empty() && currentToken.getPrecedence() <= varOpstack.front().getPrecedence()) {
                if (currentToken.getPrecedence() == varOpstack.front().getPrecedence() && varOpstack.front().getAssoc() == Token::ASOC::RIGHT)
                    break;

                varMainstack.emplace_back(varOpstack.front());
                varOpstack.pop_front();
                //qDebug() << "   pop op to varMainstack";
            }

            varOpstack.emplace_front(currentToken);
            //qDebug() << "push op to varOpstack";
            break;

        case Token::TType::LEFT_BR:
            varOpstack.emplace_front(currentToken);
            //qDebug() << "push left bracket to varOpstack";
            break;

        case Token::TType::RIGHT_BR:
            // Error: empty brackets
            if (varStr[varCursor-2] == '(' || inputStr[cursor-2] == '+' || inputStr[cursor-2] == '-' || inputStr[cursor-2] == '*'
                    || inputStr[cursor-2] == '/' || inputStr[cursor-2] == '^') {
                inputValid = false;
                ////qDebug() << "varError: empty brackets";
                break;
            }

            // pop varOpstack to varMainstack until matching parentheses found
            while(varOpstack.front().getType() != Token::TType::LEFT_BR) {
                if (varOpstack.empty()) {
                    inputValid = false;
                    ////qDebug() << "varError: no matching left brackets";
                    break;
                }
                else {
                    varMainstack.emplace_back(varOpstack.front());
                    varOpstack.pop_front();
                    //qDebug() << "   pop op inside brackets";
                }
            }

            // discard matching parentheses
            if (inputValid){
                varOpstack.pop_front();
                //qDebug() << "discard left bracket";

                // pop function beside brackets
                if (varOpstack.front().getType() == Token::TType::FUNC) {
                    varMainstack.emplace_back(varOpstack.front());
                    varOpstack.pop_front();
                    //qDebug() << "   pop function to varMainstack";
                }
            }
            break;

        case Token::TType::SIGN_NEG:
            if (negativeNum == false) {
                negativeNum = true;
            }
            else {
                negativeNum = false;
            }
            //qDebug() << "found negative sign";
            break;

        case Token::TType::SIGN_POS:
            // ignore positive sign
            //qDebug() << "found positive sign";
            break;

        case Token::TType::FUNC:
            if (inputStr[cursor] != '(') {
                inputValid = false;
                ////qDebug() << "varError: no brackets after function";
                break;
            }

            varOpstack.emplace_front(currentToken);
            //qDebug() << "found function";
            break;

        case Token::TType::COMMA:
            // ignore comma
            //qDebug() << "found comma";
            break;

        case Token::TType::ERR:
            inputValid = false;
            //qDebug() << "Error: token type error";
            break;

        default:
            inputValid = false;
            //qDebug() << "Error: default error";
            break;
        }
    }

    // pop remaining varOpstack to varMainstack
    while (!varOpstack.empty() && inputValid) {
        if (varOpstack.front().getType() == Token::TType::LEFT_BR) {
            inputValid = false;
            //qDebug() << "Error: no matching right bracket";
            break;
        }

        varMainstack.emplace_back(varOpstack.front());
        varOpstack.pop_front();
        //qDebug() << "   pop op to varMainstack to empty varOpstack";
    }

    qDebug() << "VarParser DONE: varmainstack[0]: " << varMainstack[0].getNum();

    Token temp = varReader(varMainstack);
    varToken.assignNum(temp.getNum());
    varToken.setValue(temp.getValue());

    qDebug() << "VarReader DONE: temp.getnum: " << temp.getNum();
}

Token Parser::varReader(std::deque<Token>& varMainstack) {
    qDebug() << "in var reader";

    // copy varMainstack to not change the stack
    std::deque<Token> stackCopy = varMainstack;
    Token varResult;

    if (stackCopy.size() == 0) {
        inputValid = false;
        return varResult;
    }

    //qDebug() << "VARREADER mainstack size: " << varMainstack.size();
    //qDebug() << "VARREADER: mainstack:[0] " << varMainstack[0].getNum() << ", value: " << varMainstack[0].getValue();

    qDebug() << "VARREADER stackcopy size: " << stackCopy.size();
    qDebug() << "VARREADER: stackCopy:[0] " << stackCopy[0].getNum() << ", value: " << stackCopy[0].getValue();
    //qDebug() << "[x] : " << var.variables["x"];

    for (std::deque<Token>::size_type i = 0; i < stackCopy.size() && inputValid && stackCopy.size() > 1; i++) {
        // found operator in varMainstack
        if (stackCopy[i].getType() == Token::TType::OP) {
            qDebug() << "SOLVING VARMAINSTACK: found operator";

            if (stackCopy.size() < 3) {
                inputValid = false;
                break;
            }

            // pop 2 numbers before op
            Token num1, num2, op, result;
            num1 = stackCopy[i-2];
            num2 = stackCopy[i-1];
            op = stackCopy[i];

            //qDebug() << "values num1: " << num1.getValue() << ", num2: " << num2.getValue() << ", op: " << op.getValue();
            //qDebug() << "numbers num1: " << num1.getNum() << ", num2: " << num2.getNum() << ", op: " << op.getValue();

            // do calculations
            result = calculateOp(num1, num2, op);

            // replace the calculated numbers and operator with result
            stackCopy[i] = result;
            stackCopy.erase(stackCopy.begin() + (i - 2), stackCopy.begin() + i);

            // parse again
            i = 0;
            continue;
        }

        // found function in mainstack
        if (stackCopy[i].getType() == Token::TType::FUNC) {
            qDebug() << "SOLVING MAINSTACK: found function";

            if (stackCopy.size() < 2) {
                inputValid = false;
                break;
            }

            // pop one number before function
            Token num, func, result;
            num = stackCopy[i - 1];
            func = stackCopy[i];

            // do calculations
            result = calculateFunc(num, func);

            // replace the calculated numbers and operator with result
            stackCopy[i] = result;
            stackCopy.erase(stackCopy.begin() + (i - 1), stackCopy.begin() + i);

            // parse again
            i = 0;
            continue;
        }
    }


    if (stackCopy[0].getType() == Token::TType::VAR) {
        auto it = var.variables.find(stackCopy[0].getValue());

        // variable doesn't exist
        if (it == var.variables.end()) {
            inputValid = false;
            qDebug() << "variable not found";
        }
        // variable exists
        else {
            qDebug() << "2.1 num1 variable, it->first: " << it->first << ", second: " << it->second;
            if (inputValid) stackCopy[0].setValue(it->second);
            qDebug() << "2.2 num1 variable, value: " << stackCopy[0].getValue() << ", number: ", stackCopy[0].getNum();
            qDebug() << "2.2 num1 variable, it->first: " << it->first << ", second: " << it->second;
        }
    }

    qDebug() << "varReader: stack[0] num: " << stackCopy[0].getNum() << ", value: " << stackCopy[0].getValue();

    varResult = stackCopy[0];
    return varResult;
}
