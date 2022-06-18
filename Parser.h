#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <deque>
#include <QString>

#include "Token.h"
#include "VarList.h"

class Parser {
public:
    enum class IType {
        X, Y, VAR, ERR
    };

public:
    std::deque<Token> mainStack, opStack;
    int               cursor = 0;
    Token             currentToken, resultToken;

    QString inputStr;
    QString inputVar;
    bool    inputValid = true;
    IType   inputType;

    bool    negativeNum = false;

    VarList var;
    QString outputStr;

public:
    Parser() {}
    Parser(QString);

    void    init(QString);
    void    parseInputVar();
    void    parseMainStr();
    void    parseMainStrForVar(QString);
    void    mainStackToStr();
    void    resultToStr();
    QString getOutputStr();

    long double readMainStack();
    Token       calculateOp(Token, Token, Token);
    Token       calculateFunc(Token, Token);
    Token       calculateFunc(Token, Token, Token);

    void  checkVarInputValidity();
    void  varParser(Token&);
    Token varReader(std::deque<Token>&);
};

#endif // PARSER_H
