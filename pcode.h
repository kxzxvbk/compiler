//
// Created by 23207 on 2020/11/5.
//
/*
 * add  #
 * sub  #
 * mult  #
 * div  #
 * lod name  #
 * allocate name size  #
 * initialize_a name array  #
 * li constant  #
 * const type name  #
 * var type name size  #
 * eq
 * print_s string  #
 * print_v  #
 * scanf name  #
 * set_function_flag name  #
 * assign name
 */

#ifndef UNTITLED_PCODE_H
#define UNTITLED_PCODE_H
#include "string"
#include "list"
using namespace std;

class Pcode {
public:
    Pcode(string type, string op1, string op2, string op3, vector<int> arr) {
        this->type = type;
        this->op1 = op1;
        this->op2 = op2;
        this->op3 = op3;
        this->arr = arr;
    }
    string type;
    string op1;
    string op2;
    string op3;
    vector<int> arr;
};

list<Pcode> pcodes;
list<Pcode> buffer;
static bool buffer_on = false;

void cache_emit() {
    buffer_on = true;
}

list<Pcode> get_cache() {
    return buffer;
}

void close_cache() {
    buffer_on = false;
}

void clear_cache() {
    buffer = list<Pcode>();
    buffer_on = false;
}

void emit(string type, string op1="", string op2="", string op3="") {
    if (!buffer_on) pcodes.emplace_back(type, op1, op2, op3, vector<int>());
    else {
        pcodes.emplace_back(type, op1, op2, op3, vector<int>());
        buffer.emplace_back(type, op1, op2, op3, vector<int>());
    }
    Pcode pc = pcodes.back();
    cout << pc.type << " " << pc.op1 << " " << pc.op2 << " " << pc.op3 << endl;
}

void emit(string type, vector<int> arr, string name="") {
    if (!buffer_on) pcodes.emplace_back(type, name, "", "", arr);
    else {
        pcodes.emplace_back(type, name, "", "", arr);
        buffer.emplace_back(type, name, "", "", arr);
    }
    Pcode pc = pcodes.back();
    cout << pc.type << " " << pc.op1 << " " << pc.op2 << " " << pc.op3 << endl;
}

void emit(string type, vector<int> arr, string op1, string op2, string op3) {
    if (!buffer_on) pcodes.emplace_back(type, op1, op2, op3, arr);
    else {
        pcodes.emplace_back(type, op1, op2, op3, arr);
        buffer.emplace_back(type, op1, op2, op3, arr);
    }
    Pcode pc = pcodes.back();
    cout << pc.type << " " << pc.op1 << " " << pc.op2 << " " << pc.op3 << endl;
}


#endif //UNTITLED_PCODE_H
