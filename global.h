//
// Created by 23207 on 2020/10/4.
//

#ifndef COMPILER_GLOBAL_H
#define COMPILER_GLOBAL_H
#include <iostream>
#include <fstream>
#include "symbol.h"
#include "entry.h"
using namespace std;

bool t3 = false;
bool t4 = false;

ofstream fout("output.txt");
ofstream eout("error.txt");

symbol sym = symbol(1, 1, "1", 1, 1);
Symbol_tab global_sym_tab;
#endif //COMPILER_GLOBAL_H
