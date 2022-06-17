#include <iostream>
#include <string>
#include <QDebug>

#include "Token.h"

Token::Token (double num) {
    std::string temp = std::to_string(num);
    value = QString::fromStdString(temp);

    number = num;
    type = Token::TType::NUM;
}

Token::Token (char op) {
    value = "";
    value += op;
    type = Token::TType::OP;
}

void Token::setPrecedence() {
    if (value == "+" || value == "-") {
        precedence = 1;
    }
    else if (value == "*" || value == "/") {
        precedence = 2;
    }
    else if (value == "^") {
        precedence = 3;
    }
    else if (value == "(" || value == ")" || type == Token::TType::FUNC) {
        precedence = 0;
    }
}

int Token::getPrecedence() {
    return precedence;
}

Token::TType Token::getType() {
    return type;
}

void Token::setValue(QString input) {
    value = input;
}

QString Token::getValue() {
    return value;
}

Token::ASOC Token::getAssoc() {
    return associative;
}

double Token::getNum() {
    return number;
}

Token::TType Token::getNextToken(QString input, int& cursor) {
    value = "";

    if (input[cursor].isDigit() || input[cursor] == '.') {
        type = Token::TType::NUM;
        int dotCount = 0;

        for (int i = cursor; i < input.length() && (input[i].isDigit() || input[i] == '.'); i++) {
            // check comma validity
            if (input[i] == '.') {
                dotCount++;
            }
            if (dotCount > 1) {
                type = Token::TType::ERR;
                return type;
            }

            value += input[i];
            cursor++;
        }

        number = std::stod(value.toStdString());
        return type;
    }

    if (isAlphabet(input[cursor])) {
        for (int i = cursor; i < input.length() && (isAlphabet(input[i]) || input[i].isDigit()); i++) {
            value += input[i];
            cursor++;
        }

        // check if value is function or variable
        if (value == "x" || value == "y" || value == "sin" || value ==  "cos" || value == "tan" || value == "pow" || value == "sqrt") {
            type = Token::TType::FUNC;
            setPrecedence();
            associative = Token::ASOC::FUNC;
        }
        else {
            type = Token::TType::VAR;
        }

        return type;
    }

    if (input[cursor] == '(') {
        type = Token::TType::LEFT_BR;
        value += input[cursor];
        setPrecedence();
        cursor++;
        return type;
    }

    if (input[cursor] == ')') {
        type = Token::TType::RIGHT_BR;
        value += input[cursor];
        setPrecedence();
        cursor++;
        return type;
    }

    if (input[cursor] == '*' || input[cursor] == '/' || input[cursor] == '^') {
        type = Token::TType::OP;

        // input value and precedence
        value += input[cursor];
        setPrecedence();

        // set associativity
        if (input[cursor] == '^') {
            associative = ASOC::RIGHT;
        }
        else {
            associative = ASOC::LEFT;
        }

        cursor++;
        return type;
    }

    if (input[cursor] == '+' || input[cursor] == '-') {
        // check if +- is operator
        if (input[cursor - 1] == ')' || input[cursor - 1].isDigit() || isAlphabet(input[cursor - 1])) {
            type = Token::TType::OP;

            // input value and precedence and associativity
            value += input[cursor];
            setPrecedence();
            associative = ASOC::LEFT;

            cursor++;
            return type;
        }

        // +- is sign
        if (input[cursor] == '-') {
            type = Token::TType::SIGN_NEG;
        }
        else {
            type = Token::TType::SIGN_POS;
        }
        value += input[cursor];
        cursor++;
        return type;

    }

    return Token::TType::ERR;
}

void Token::assignNum(double num) {
    std::string temp = std::to_string(num);
    value = QString::fromStdString(temp);

    number = num;
    type = Token::TType::NUM;
}

void Token::valueToNum(QString str) {
    number = std::stod(str.toStdString());
}

bool Token::isAlphabet(QString input) {
    for (int i = 0; i < input.length(); i++) {
        if ((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')) {
            continue;
        }
        else {
            return false;
        }
    }

    return true;
}

bool Token::isAlphabet(char input) {
    return (input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z');
}

bool Token::isQDigit(QString input) {
    for (int i = 0; i < input.length(); i++) {
        if (!input[i].isDigit()) {
            return false;
        }
    }

    return true;
}

