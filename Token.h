#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <QString>

class Token {
public:
    enum class TType {
        X, Y, NUM, VAR, SIGN_POS, SIGN_NEG, LEFT_BR, RIGHT_BR, FUNC, OP, EQUAL, COMMA, ERR
    };

    enum class ASOC {
        RIGHT, LEFT, FUNC
    };

private:
    TType   type;
    QString value;
    double  number;
    int     precedence;
    ASOC    associative;


public:
    Token() {}
    Token(QString);
    Token(double);
    Token(char);

    void    setPrecedence();
    int     getPrecedence();
    TType   getType();
    void    setValue(QString);
    QString getValue();
    ASOC    getAssoc();
    double  getNum();
    TType   getNextToken(QString, int&);

    void assignNum(double);
    void valueToNum(QString);

    bool isAlphabet(QString);
    bool isAlphabet(char);
    bool isQDigit(QString);
};

#endif // TOKEN_H
