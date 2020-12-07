//
// Created by 23207 on 2020/11/21.
//

#ifndef UNTITLED_MIPS_CODE_H
#define UNTITLED_MIPS_CODE_H
#include "string"
#include "global.h"
#include "mips_command.h"
using namespace std;

class m_command {
public:
    string type;
    string op1;
    string op2;
    string op3;
    m_command(string type, string op1="", string op2="", string op3="") {
        this->type = type;
        this->op1 = op1;
        this->op2 = op2;
        this->op3 = op3;
    }

    void output_to_file() {
    }
};

#endif //UNTITLED_MIPS_CODE_H
