
//
// Created by 23207 on 2020/10/4.
//

#ifndef COMPILER_GLOBAL_ERROR_H
#define COMPILER_GLOBAL_ERROR_H
#include "global.h"
using namespace std;


class global_error {
public:
    global_error(int line, char c) {
        this->type = c;
        this->line = line;
    }

    void output() const {
        if (t4) eout << this->line << ' ' << this->type << endl;
    }

    int get_line() const {
        return this->line;
    }

private:
    char type;
    int line;
};


#endif //COMPILER_GLOBAL_ERROR_H
