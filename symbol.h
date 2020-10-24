//
// Created by 23207 on 2020/9/20.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H
#include <string>
#include <utility>
using namespace std;
const char* symbol_type[39] = {
        // 0-3
        "IDENFR", "INTCON", "CHARCON", "STRCON",
        // 4-18
        "CONSTTK", "INTTK", "CHARTK", "VOIDTK", "MAINTK", "IFTK", "ELSETK", "SWITCHTK", "CASETK",
        "DEFAULTTK", "WHILETK", "FORTK", "SCANFTK", "PRINTFTK", "RETURNTK",
        // 19-22
        "PLUS", "MINU", "MULT", "DIV",
        // 23-28
        "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ",
        // 29-38
        "COLON", "ASSIGN", "SEMICN", "COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"
};
const int rwn = 15;  // reserved words' number
const char* reserved_words[rwn] = {
        "case", "char", "const", "default", "else", "for", "if", "int", "main",
        "printf", "return", "scanf", "switch", "void", "while"
};  // **this array should be sorted**

const int reserved2id[rwn] {
    12, 6, 4, 13, 10, 15, 9, 5, 8, 17, 18, 16, 11, 7, 14
};

class symbol {
public:
    symbol(int type, int value, string source, int line_n, int col_n);
    int get_type() const;
    int get_line_n() const;
    int get_col_n() const;
    int get_value() const;
    string get_source() const;
private:
    int type;
    int value;
    int line_n;
    int col_n;
    string source;
};

symbol::symbol(int type, int value, string source, int line_n, int col_n)  {
    this->type = type;
    this->value = value;
    this->source = source;
    this->line_n = line_n;
    this->col_n = col_n;
}

int symbol::get_type() const {
    return this->type;
}

int symbol::get_col_n() const {
    return this->col_n;
}

int symbol::get_line_n() const {
    return this->line_n;
}

int symbol::get_value() const {
    return this->value;
}

string symbol::get_source() const {
    return this->source;
}

#endif //COMPILER_SYMBOL_H
