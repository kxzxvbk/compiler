//
// Created by 23207 on 2020/10/4.
//

#ifndef COMPILER_PIPELINE_H
#define COMPILER_PIPELINE_H
#include "word_parsing.h"
#include "grammar.h"
#include "mips.h"
#include "flow_graph.h"

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

void task4() {
    // error processing
    t4 = true;
    parse_symbols();
    list_it = symbols.cbegin();
    sym = getsym();
    g_programme();
    output_errors();
}

void task5() {
    // semantic analysis & code generation
    t5 = true;
    parse_symbols();
    list_it = symbols.cbegin();
    sym = getsym();
    g_programme();
    pcode2quardcode();
    output_qcodes_to_file();
    if (o2) {
        optimization_for_qcodes();
        output_optimized_qcodes_to_file();
    }
    qcodes2mips();
}

#endif //COMPILER_PIPELINE_H
