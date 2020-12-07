//
// Created by 23207 on 2020/11/6.
//

#ifndef UNTITLED_MIPS_COMMAND_H
#define UNTITLED_MIPS_COMMAND_H
#include "string"
#include "global.h"

string blank = ", ";
static int reserved = 0;
string split_reg_src_from_lw_and_sw(string origin) {
    string ans = "";
    if (origin.substr(0, 3) != "sw " && origin.substr(0, 3) != "lw ") return ans;
    return origin.substr(3, 3);
}

static string last_output;
void lw(const string& reg_name, int offset, const string& base_reg) {
    string ans;
    if ((last_output == "sw " + reg_name + blank + to_string(offset) +
    "(" + base_reg + ")" && o1)) return;
    else if (last_output == "sw " + split_reg_src_from_lw_and_sw(last_output)
    + blank + to_string(offset) + "(" + base_reg + ")" && o1)
        ans = "move " + reg_name + blank + split_reg_src_from_lw_and_sw(last_output);
    else ans =  "lw " + reg_name + blank + to_string(offset) + "(" + base_reg + ")";
    mout << ans << endl;
    last_output = ans;
}

void lw(const string& reg_name, const string& off_reg, const string& base_reg) {
    string ans1, ans2;
    ans1 =  "add " + off_reg + blank + base_reg + blank + off_reg;
    ans2 =  "lw " + reg_name + ", (" + off_reg + ")";
    mout << ans1 << endl;
    mout << ans2 << endl;
    last_output = ans2;
}

void sw(const string& reg_name, int offset, const string& base_reg) {
    string ans;
    ans = "sw "  + reg_name + blank + to_string(offset) + "(" + base_reg + ")";
    mout << ans << endl;
    last_output = ans;
}

void sw(const string& reg_name, const string& off_reg, const string& base_reg) {
    string ans1, ans2;
    ans1 =  "add " + off_reg + blank + base_reg + blank + off_reg;
    ans2 =  "sw " + reg_name + ", (" + off_reg + ")";
    mout << ans1 << endl;
    mout << ans2 << endl;
    last_output = ans2;
}

void sb(const string& reg_name, int offset, const string& base_reg) {
    string ans;
    ans =  "sb " + reg_name + blank + to_string(offset) + "(" + base_reg + ")";
    mout << ans << endl;
    last_output = ans;
}

void add(const string& reg_name_dst, const string& reg_name1, const string& reg_name2) {
    string ans;
    ans =  "addu " + reg_name_dst + blank + reg_name1 + blank + reg_name2;
    mout << ans << endl;
    last_output = ans;
}

void addi(const string& reg_name_dst, const string& reg_name1, const string& constant) {
    string ans;
    ans =  "addiu " + reg_name_dst + blank + reg_name1 + blank + constant;
    mout << ans << endl;
    last_output = ans;
}

void sub(const string& reg_name_dst, const string& reg_name1, const string& reg_name2) {
    string ans;
    ans =  "sub " + reg_name_dst + blank + reg_name1 + blank + reg_name2;
    mout << ans << endl;
    last_output = ans;
}

void subi(const string& reg_name_dst, const string& reg_name1, const string& constant) {
    string ans;
    ans =  "subi " + reg_name_dst + blank + reg_name1 + blank + constant;
    mout << ans << endl;
    last_output = ans;
}

void mult(const string& reg_name1, const string& reg_name2) {
    string ans;
    ans =  "mult " + reg_name1 + blank + reg_name2;
    mout << ans << endl;
    last_output = ans;
}

void neg(const string& reg_name1, const string& reg_name2) {
    string ans;
    ans =  "neg " + reg_name1 + blank + reg_name2;
    mout << ans << endl;
    last_output = ans;
}

void div(const string& reg_name1, const string& reg_name2) {
    string ans;
    ans =  "div " + reg_name1 + blank + reg_name2;
    mout << ans << endl;
    last_output = ans;
}

void mflo(const string& reg_name) {
    string ans;
    ans =  "mflo " + reg_name;
    mout << ans << endl;
    last_output = ans;
}

void li(const string& reg_name, const string& num) {
    string ans;
    ans =  "li " + reg_name + blank + num;
    mout << ans << endl;
    last_output = ans;
}

void syscall() {
    string ans;
    ans =  "syscall";
    mout << ans << endl;
    last_output = ans;
}

void set_flag(const string& flag_name) {
    string ans;
    ans =  flag_name + ":";
    mout << ans << endl;
    last_output = ans;
}

void set_data_seg() {
    string ans;
    ans =  ".data";
    mout << ans << endl;
    last_output = ans;
}

void set_text_seg() {
    string ans;
    ans =  ".text";
    mout << ans << endl;
    last_output = ans;
}

void j(const string& label_name) {
    string ans;
    ans =  "j " + label_name;
    mout << ans << endl;
    last_output = ans;
}

void allocate(const string& name, const string& size) {
    string ans;
    ans =  name + ":    .space    " + size;
    mout << ans << endl;
    last_output = ans;
}

void allocate_init(const string& type, const string& name, vector<int> array) {
    string ans;
    ans =  name + ":    .word    ";
    int n = array.size();
    ans +=  to_string(array[0]);
    for (int i = 1;i < n;i++) {
        ans +=  blank + to_string(array[i]);
    }
    mout << ans << endl;
    last_output = ans;
}

void la(const string& reg_name, const string& label_name) {
    string ans;
    ans =  "la " + reg_name + blank + label_name;
    mout << ans << endl;
    last_output = ans;
}

void branch(const string& type, const string& op1, const string& op2, const string& label) {
    string ans;
    ans =  type + " " + op1 + blank + op2 + blank + label;
    mout << ans << endl;
    last_output = ans;
}

void jal(const string& label_name) {
    string ans;
    ans =  "jal " + label_name;
    mout << ans << endl;
    last_output = ans;
}

void jr(const string& reg_name) {
    string ans;
    ans =  "jr " + reg_name;
    mout << ans << endl;
    last_output = ans;
}

void allocate_string(const string& string_name, const string& content) {
    string ans;
    ans =  string_name + ":    .asciiz    \"" + content + "\"";
    mout << ans << endl;
    last_output = ans;
}

void mv(const string& reg_dst, const string& reg_src) {
    string ans;
    ans =  "move " + reg_dst + blank + reg_src;
    mout << ans << endl;
    last_output = ans;
}

void sll(const string& reg_name_dst, const string& reg_name1, const string& constant) {
    string ans;
    ans =  "sll " + reg_name_dst + blank + reg_name1 + blank + constant;
    mout << ans << endl;
    last_output = ans;
}

void sra(const string& reg_name_dst, const string& reg_name1, const string& constant) {
    string ans;
    string reserved_name = "RESERVED_" + to_string(reserved);
    reserved++;

    ans = "bgez " + reg_name1 + blank + reserved_name;
    mout << ans << endl;
    ans = "neg " + reg_name1 + blank + reg_name1;
    mout << ans << endl;
    ans =  "sra " + reg_name_dst + blank + reg_name1 + blank + constant;
    mout << ans << endl;
    ans = "neg " + reg_name_dst + blank + reg_name_dst;
    mout << ans << endl;

    mout << "j RESERVED_" + to_string(reserved) << endl;
    mout << reserved_name + ":" << endl;
    reserved_name = "RESERVED_" + to_string(reserved);
    reserved++;
    ans =  "sra " + reg_name_dst + blank + reg_name1 + blank + constant;
    mout << ans << endl;
    mout << reserved_name + ":" << endl;
    last_output = ans;
}

#endif //UNTITLED_MIPS_COMMAND_H
