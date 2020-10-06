//
// Created by 23207 on 2020/10/4.
//

#ifndef COMPILER_PIPELINE_H
#define COMPILER_PIPELINE_H
#include "word_parsing.h"
#include "grammar.h"

void task2() {
    // lexical analysis
    parse_symbols();
    output_symbols_to_file();
    // you can use the following function to DEBUG.
    // output_symbols_to_console();
}

void task3() {
    // grammar analysis
    t3 = true;
    parse_symbols();
    list_it = symbols.cbegin();
    sym = getsym();
    g_programme();
}

#endif //COMPILER_PIPELINE_H
