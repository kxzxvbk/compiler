//
// Created by 23207 on 2020/11/6.
//

#ifndef UNTITLED_MIPS_COMMAND_H
#define UNTITLED_MIPS_COMMAND_H
#include "string"
#include "global.h"

string blank = ", ";
void lw(const string& reg_name, int offset, const string& base_reg) {
    if (base_reg == "$sp") mout << "lw " + reg_name + blank + to_string(-offset) + "(" + base_reg + ")" << endl;
    else mout << "lw " + reg_name + blank + to_string(offset-4) + "(" + base_reg + ")" << endl;
}

void lw(const string& reg_name, const string& off_reg, const string& base_reg) {
    if (base_reg == "$sp") {
        mout << "neg " + off_reg + blank + off_reg << endl;
        mout << "lw " + reg_name + blank + off_reg + "(" + base_reg + ")" << endl;
    }
    else mout << "lw " + reg_name + blank + off_reg + "(" + base_reg + ")" << endl;
}

void sw(const string& reg_name, int offset, const string& base_reg) {
    if (base_reg == "$sp") mout << "sw " + reg_name + blank + to_string(-offset) + "(" + base_reg + ")" << endl;
    else mout << "sw " + reg_name + blank + to_string(offset-4) + "(" + base_reg + ")" << endl;
}

void sw(const string& reg_name, const string& off_reg, const string& base_reg) {
    if (base_reg == "$sp") {
        mout << "neg " + off_reg + blank + off_reg << endl;
        mout << "sw " + reg_name + blank + off_reg + "(" + base_reg + ")" << endl;
    }
    else mout << "sw " + reg_name + blank + off_reg + "(" + base_reg + ")" << endl;
}

void sb(const string& reg_name, int offset, const string& base_reg) {
    if (base_reg == "$sp") mout << "sb " + reg_name + blank + to_string(-offset) + "(" + base_reg + ")" << endl;
    else mout << "sb " + reg_name + blank + to_string(offset) + "(" + base_reg + ")" << endl;
}

void add(const string& reg_name_dst, const string& reg_name1, const string& reg_name2) {
    mout << "add " + reg_name_dst + blank + reg_name1 + blank + reg_name2 << endl;
}

void addi(const string& reg_name_dst, const string& reg_name1, const string& constant) {
    mout << "addi " + reg_name_dst + blank + reg_name1 + blank + constant << endl;
}

void sub(const string& reg_name_dst, const string& reg_name1, const string& reg_name2) {
    mout << "sub " + reg_name_dst + blank + reg_name1 + blank + reg_name2 << endl;
}

void subi(const string& reg_name_dst, const string& reg_name1, const string& constant) {
    mout << "subi " + reg_name_dst + blank + reg_name1 + blank + constant << endl;
}

void mult(const string& reg_name1, const string& reg_name2) {
    mout << "mult " + reg_name1 + blank + reg_name2 << endl;
}

void neg(const string& reg_name1, const string& reg_name2) {
    mout << "neg " + reg_name1 + blank + reg_name2 << endl;
}

void div(const string& reg_name1, const string& reg_name2) {
    mout << "div " + reg_name1 + blank + reg_name2 << endl;
}

void mflo(const string& reg_name) {
    mout << "mflo " << reg_name << endl;
}

void li(const string& reg_name, const string& num) {
    mout << "li " + reg_name + blank + num << endl;
}

void syscall() {
    mout << "syscall" << endl;
}

void set_flag(const string& flag_name) {
    mout << flag_name + ":" << endl;
}

void set_data_seg() {
    mout << ".data" << endl;
}

void set_text_seg() {
    mout << ".text" << endl;
}

void j(const string& label_name) {
    mout << "j " + label_name << endl;
}

void allocate(const string& name, const string& size) {
    mout << name + ":    .space    " + size << endl;
}

void allocate_init(const string& type, const string& name, vector<int> array) {
    mout << name + ":    .word    ";
    int n = array.size();
    mout << array[0];
    for (int i = 1;i < n;i++) {
        mout << blank + to_string(array[i]);
    }
    mout << endl;
}

void la(const string& reg_name, const string& label_name) {
    mout << "la " + reg_name + blank + label_name << endl;
}

void branch(const string& type, const string& op1, const string& op2, const string& label) {
    mout << type + " " + op1 + blank + op1 + blank + label;
}

#endif //UNTITLED_MIPS_COMMAND_H
