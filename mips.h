//
// Created by 23207 on 2020/11/4.
//

/*
 * when func_return:
 *    set $v0
 *    jr $ra
 *    lw $ra, $v1
 *
 *
 * when entering a function:
 *    sw $ra, STACK_TOP($sp)
 *    set spaces for:
 *
 * when assignment or initialization:
 *    <some operations>
 *    sw $s, OFFSET($sp)
 *
 * when calling a function:
 *    subi $sp, $sp, TOP_INDEX
 *    jal CALLEE
 *    addi $sp, $sp, TOP_INDEX
 */

#ifndef UNTITLED_MIPS_H
#define UNTITLED_MIPS_H
#include "quard_code.h"
#include "global.h"
#include "mips_command.h"
#include <sstream>

static string random_key = "Wh1tBA12H_";

void write_back_all_t();
void write_back_all_s();
void flush_reservation();
void clear_all();
string get_allocated_reg(const string& name);
string write_allocate_reg(const string& name, bool replace=false);
string read_allocate_reg(const string& name, bool for_write=false);
string allocate_reg_and_load(const string& var_name);
void save_spot();


map<string, int> temp_sym_tab;
map<string ,bool> temp_sym_glob;
map<string, bool> temp_sym_const;
map<string, int> temp_sym_focus;
map<string, int> temp_func_tab;
string cur_func_name = "";
int top_index = 0;
int over_flow_index = 0;
int current_top = 0;
int down_move_length;

int str2int(string in) {
    stringstream ss;
    ss << in;
    int size;
    ss >> size;
    return size;
}

int get_log(int x) {
    if (x < 0) x = -x;
    int pow = 0;
    if (x == 1 || x == 0) return -1;
    while (x >= 2) {
        if (x % 2 != 0) return -1;
        x /= 2;
        pow++;
    }
    return pow;
}

bool start_with(string ss, string prefix) {
    if (prefix.size() > ss.size()) return false;
    for (int i = 0;i < prefix.size();i++) {
        if (prefix[i] != ss[i]) return false;
    }
    return true;
}

void push_tab(const string& name, int size, bool is_global) {
    top_index += 4;
    temp_sym_tab[name] = top_index;
    temp_sym_glob[name] = is_global;
    temp_sym_const[name] = false;
    if (size - 4 > 0) {
        top_index += size - 4;
    }
}

void push_quot(const string& new_name, const string& old_name, bool is_global) {
    temp_sym_tab[new_name] = temp_sym_tab[old_name];
    temp_sym_glob[new_name] = is_global;

    if (temp_sym_const[old_name]) {
        temp_sym_const[new_name] = true;
        temp_sym_focus[new_name] = temp_sym_focus[old_name];
    }
}

void push_func(const string& name) {
    temp_func_tab[name] = top_index;
}

void push_const(const string& name, int value) {
    temp_sym_const[name] = true;
    temp_sym_focus[name] = value;
}

bool is_temp_var(const string& name);

static int string_count = 0;
static int s_count = 0;
void set_string() {
    list<Quardcode>::const_iterator iterator;
    for (iterator = qcodes.cbegin(); iterator != qcodes.cend(); iterator++) {
        Quardcode pc = *iterator;
        if (pc.type == "print_s") {
            string string_name = random_key + "string_" + to_string(string_count);
            string_count++;
            allocate_string(string_name, pc.dst);
        }
    }
}

void set_temp_sym_tab() {
    list<Quardcode>::const_iterator iterator;
    bool is_glob = true;
    push_const("std::endl", int('\n'));
    for (iterator = qcodes.cbegin(); iterator != qcodes.cend(); iterator++) {
        Quardcode pc = *iterator;

        if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "div") {
            // 出现非单的情况
            if (pc.dst == pc.op1 || pc.dst == pc.op2) {
                if (is_temp_var(pc.dst) && temp_sym_const[pc.op1] && temp_sym_const[pc.op2] && o1) {
                    if (pc.type == "add") push_const(pc.dst, temp_sym_focus[pc.op1] + temp_sym_focus[pc.op2]);
                    else if (pc.type == "sub") push_const(pc.dst, temp_sym_focus[pc.op1] - temp_sym_focus[pc.op2]);
                    else if (pc.type == "mult") push_const(pc.dst, temp_sym_focus[pc.op1] * temp_sym_focus[pc.op2]);
                    else if (pc.type == "div") push_const(pc.dst, temp_sym_focus[pc.op1] / temp_sym_focus[pc.op2]);
                }
            }
            else if (temp_sym_const[pc.op1] && temp_sym_const[pc.op2] && o1 && is_temp_var(pc.dst)) {
                if (pc.type == "add") push_const(pc.dst, temp_sym_focus[pc.op1] + temp_sym_focus[pc.op2]);
                else if (pc.type == "sub") push_const(pc.dst, temp_sym_focus[pc.op1] - temp_sym_focus[pc.op2]);
                else if (pc.type == "mult") push_const(pc.dst, temp_sym_focus[pc.op1] * temp_sym_focus[pc.op2]);
                else if (pc.type == "div") push_const(pc.dst, temp_sym_focus[pc.op1] / temp_sym_focus[pc.op2]);
            }
            else if (temp_sym_tab.find(pc.dst) != temp_sym_tab.end());
            else push_tab(pc.dst, 4, is_glob);
        }
        else if (pc.type == "multi_rep" ) {
            if (temp_sym_const[pc.op1] && o1) {
                push_const(pc.dst, str2int(pc.op2) * temp_sym_focus[pc.op1]);
            }
            else push_tab(pc.dst, 4, is_glob);
        }
        else if (pc.type == "neg") {
            if (temp_sym_const[pc.op1] && o1) {
                push_const(pc.dst, -temp_sym_focus[pc.op1]);
            }
            else push_tab(pc.dst, 4, is_glob);
        }
        else if (pc.type == "li") {
            push_const(pc.dst, str2int(pc.op1));
        }
        else if (pc.type == "allocate"
                 || pc.type == "allocate_init" || pc.type == "var") {
            int size = str2int(pc.op2);
            push_tab(pc.op1, size, is_glob);
        } else if (pc.type == "set_function_flag") {
            is_glob = false;
            if (cur_func_name != "") push_func(cur_func_name);
            cur_func_name = pc.dst;
            temp_sym_tab[pc.dst + "_$ra"] = 0;
            temp_sym_glob[pc.dst + "_$ra"] = false;
            top_index = 0;
        } else if (pc.type == "lod") {
            if (!start_with(pc.op1, "@")) push_tab(pc.dst, 4, is_glob);
            else push_quot(pc.dst, pc.op1, is_glob);
        } else if (pc.type == "lod_off" || pc.type == "para") {
            push_tab(pc.dst, 4, is_glob);
        } else if (pc.type == "call") {
            if (pc.op1 != "@None") push_tab(pc.op1, 4, is_glob);
        } else if (pc.type == "assign") {
            if (temp_sym_tab.find(pc.dst) == temp_sym_tab.end() && is_temp_var(pc.dst)) {
                if (temp_sym_const[pc.op1]) push_const(pc.dst, temp_sym_focus[pc.op1]);
                else push_tab(pc.dst, 4, is_glob);
            }
        }
    }
    push_func("main");
}

string data_base = "$a3";

void qcodes2mips() {
    set_temp_sym_tab();
    list<Quardcode>::const_iterator iterator;
    bool no_func = true;
    set_data_seg();
    allocate(random_key + "_reserved_data_base", "4");

    for (iterator = qcodes.cbegin(); iterator != qcodes.cend(); iterator++) {
        Quardcode pc = *iterator;
        flush_reservation();
        mout << "# " + pc.to_string() << endl;
        if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "div"
            || pc.type == "multi_rep") {
            current_top += 4;
            if (pc.type == "multi_rep") {
                int addr = temp_sym_tab[pc.op1];
                string base_reg = "$sp";
                string reg_name_dst;

                if (temp_sym_glob[pc.op1]) base_reg = data_base;

                int addr1 = temp_sym_tab[pc.dst];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg = data_base;

                if (temp_sym_const[pc.op1] && o1) {
                    if (temp_sym_const[pc.dst]);
                    else {
                        reg_name_dst = "$t0";
                        if (o3) reg_name_dst = write_allocate_reg(pc.dst);
                        li(reg_name_dst, to_string(temp_sym_focus[pc.op1] * str2int(pc.op2)));
                        if (!o3) sw(reg_name_dst, temp_sym_tab[pc.dst], base_reg);
                    }
                }
                else {
                    string reg_name0;
                    string reg_name1;

                    reg_name0 = "$t0";
                    if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
                    else lw(reg_name0, addr, base_reg);
                    int con = str2int(pc.op2);

                    reg_name_dst = "$t2";
                    if (o3) reg_name_dst = write_allocate_reg(pc.dst);
                    if (o1 && get_log(con) > 0) {
                        if (con < 0) {
                            neg("$a2", reg_name0);
                            sll(reg_name_dst, "$a2", to_string(get_log(con)));
                        }
                        else sll(reg_name_dst, reg_name0, to_string(get_log(con)));
                    }
                    else if (pc.op2 != "1" || !o1) {
                        reg_name1 = "$t1";
                        if (o3) {
                            reg_name1 = read_allocate_reg("##TEMP##");
                            li(reg_name1, pc.op2);
                        }
                        else li(reg_name1, pc.op2);
                        mult(reg_name0, reg_name1);
                        mflo(reg_name_dst);
                    } else {
                        mv(reg_name_dst, reg_name0);
                    }
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                }
            }
            else if (!temp_sym_const[pc.op2] && !temp_sym_const[pc.op1]) {
                int addr1 = temp_sym_tab[pc.dst];
                int addr2 = temp_sym_tab[pc.op1];
                int addr3 = temp_sym_tab[pc.op2];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
                string base_reg2 = "$sp";
                if (temp_sym_glob[pc.op1]) base_reg2 = data_base;
                string base_reg3 = "$sp";
                if (temp_sym_glob[pc.op2]) base_reg3 = data_base;

                string reg_name0;
                string reg_name1;
                string reg_name_dst;

                reg_name0 = "$t0";
                if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
                else lw(reg_name0, addr2, base_reg2);
                reg_name1 = "$t1";
                if (o3) reg_name1 = allocate_reg_and_load(pc.op2);
                else lw(reg_name1, addr3, base_reg3);
                reg_name_dst = "$t2";
                if (o3) reg_name_dst = write_allocate_reg(pc.dst);

                if (pc.type == "add") {
                    add(reg_name_dst, reg_name0, reg_name1);
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                } else if (pc.type == "sub") {
                    sub(reg_name_dst, reg_name0, reg_name1);
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                } else if (pc.type == "mult") {
                    mult(reg_name0, reg_name1);
                    mflo(reg_name_dst);
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                } else if (pc.type == "div") {
                    div(reg_name0, reg_name1);
                    mflo(reg_name_dst);
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                }
            }
            else if (!temp_sym_const[pc.op1]){
                int addr1 = temp_sym_tab[pc.dst];
                int addr2 = temp_sym_tab[pc.op1];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
                string base_reg2 = "$sp";
                if (temp_sym_glob[pc.op1]) base_reg2 = data_base;

                string reg_name0;
                string reg_name1;
                string reg_name_dst;

                reg_name0 = "$t0";
                if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
                else lw(reg_name0, addr2, base_reg2);
                reg_name_dst = "$t2";
                if (o3) reg_name_dst = write_allocate_reg(pc.dst);

                if (pc.type == "add") {
                    if (temp_sym_focus[pc.op2] == 0 && o1) {
                        mv(reg_name_dst, reg_name0);
                        if (!o3) sw(reg_name0, addr1, base_reg1);
                    }
                    else {
                        addi(reg_name_dst, reg_name0, to_string(temp_sym_focus[pc.op2]));
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                } else if (pc.type == "sub") {
                    if (temp_sym_focus[pc.op2] == 0 && o1) {
                        mv(reg_name_dst, reg_name0);
                        if (!o3) sw(reg_name0, addr1, base_reg1);
                    }
                    else {
                        subi(reg_name_dst, reg_name0, to_string(temp_sym_focus[pc.op2]));
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                } else if (pc.type == "mult") {
                    if (temp_sym_focus[pc.op2] == 1 && o1) {
                        mv(reg_name_dst, reg_name0);
                        if (!o3) sw(reg_name0, addr1, base_reg1);
                    }
                    else if (o1 && get_log(temp_sym_focus[pc.op2]) > 0) {
                        if (temp_sym_focus[pc.op2] < 0) {
                            neg("$a2", reg_name0);
                            sll(reg_name_dst, "$a2", to_string(get_log(temp_sym_focus[pc.op2])));
                        }
                        else sll(reg_name_dst, reg_name0, to_string(get_log(temp_sym_focus[pc.op2])));
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                    else {
                        reg_name1 = "$t1";
                        if (o3) reg_name1 = allocate_reg_and_load(pc.op2);
                        else li(reg_name1, to_string(temp_sym_focus[pc.op2]));
                        mult(reg_name0, reg_name1);
                        mflo(reg_name_dst);
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                } else if (pc.type == "div") {
                    if (temp_sym_focus[pc.op2] == 1 && o1) {
                        mv(reg_name_dst, reg_name0);
                        if (!o3) sw(reg_name0, addr1, base_reg1);
                    }
                    else if (o1 && get_log(temp_sym_focus[pc.op2]) > 0) {
                        if (temp_sym_focus[pc.op2] < 0) {
                            neg("$a2", reg_name0);
                            sra(reg_name_dst, "$a2", to_string(get_log(temp_sym_focus[pc.op2])));
                        }
                        else sra(reg_name_dst, reg_name0, to_string(get_log(temp_sym_focus[pc.op2])));
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                    else {
                        reg_name1 = "$t1";
                        if (o3) reg_name1 = allocate_reg_and_load(pc.op2);
                        else li(reg_name1, to_string(temp_sym_focus[pc.op2]));
                        div(reg_name0, reg_name1);
                        mflo(reg_name_dst);
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                }
            }
            else if (!temp_sym_const[pc.op2]) {
                int addr1 = temp_sym_tab[pc.dst];
                int addr2 = temp_sym_tab[pc.op2];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
                string base_reg2 = "$sp";
                if (temp_sym_glob[pc.op2]) base_reg2 = data_base;

                string reg_name0;
                string reg_name1;
                string reg_name_dst;

                reg_name0 = "$t0";
                if (o3) reg_name0 = allocate_reg_and_load(pc.op2);
                else lw(reg_name0, addr2, base_reg2);
                reg_name_dst = "$t2";
                if (o3) reg_name_dst = write_allocate_reg(pc.dst);

                if (pc.type == "add") {
                    if (temp_sym_focus[pc.op1] == 0 && o1) {
                        mv(reg_name_dst, reg_name0);
                        if (!o3) sw(reg_name0, addr1, base_reg1);
                    }
                    else {
                        addi(reg_name_dst, reg_name0, to_string(temp_sym_focus[pc.op1]));
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                } else if (pc.type == "sub") {
                    reg_name1 = "$t1";
                    if (o3) reg_name1 = allocate_reg_and_load(pc.op1);
                    else li(reg_name1, to_string(temp_sym_focus[pc.op1]));
                    sub(reg_name_dst, reg_name1, reg_name0);
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                } else if (pc.type == "mult") {
                    if (temp_sym_focus[pc.op1] == 1 && o1) {
                        mv(reg_name_dst, reg_name0);
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                    else if (o1 && get_log(temp_sym_focus[pc.op1]) > 0) {
                        if (temp_sym_focus[pc.op1] < 0) {
                            neg("$a2", reg_name0);
                            sll(reg_name_dst, "$a2", to_string(get_log(temp_sym_focus[pc.op1])));
                        }
                        else sll(reg_name_dst, reg_name0, to_string(get_log(temp_sym_focus[pc.op1])));
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                    else {
                        reg_name1 = "$t1";
                        if (o3) reg_name1 = allocate_reg_and_load(pc.op1);
                        else li(reg_name1, to_string(temp_sym_focus[pc.op1]));
                        mult(reg_name0, reg_name1);
                        mflo(reg_name_dst);
                        if (!o3) sw(reg_name_dst, addr1, base_reg1);
                    }
                } else if (pc.type == "div") {
                    reg_name1 = "$t1";
                    if (o3) reg_name1 = allocate_reg_and_load(pc.op1);
                    else li(reg_name1, to_string(temp_sym_focus[pc.op1]));
                    div(reg_name1,reg_name0);
                    mflo(reg_name_dst);
                    if (!o3) sw(reg_name_dst, addr1, base_reg1);
                }
            }
            else if (o1) {
                string reg_name_dst;
                if (temp_sym_const[pc.dst]);
                else {
                    reg_name_dst = "$t0";
                    if (o3) reg_name_dst = write_allocate_reg(pc.dst);
                    string base_reg = "$sp";
                    if (temp_sym_glob[pc.dst]) base_reg = data_base;
                    if (pc.type == "add") li(reg_name_dst, to_string(temp_sym_focus[pc.op1] + temp_sym_focus[pc.op2]));
                    else if (pc.type == "sub") li(reg_name_dst, to_string(temp_sym_focus[pc.op1] - temp_sym_focus[pc.op2]));
                    else if (pc.type == "mult") li(reg_name_dst, to_string(temp_sym_focus[pc.op1] * temp_sym_focus[pc.op2]));
                    else if (pc.type == "div") li(reg_name_dst, to_string(temp_sym_focus[pc.op1] / temp_sym_focus[pc.op2]));
                    if (!o3) sw(reg_name_dst, temp_sym_tab[pc.dst], base_reg);
                }
            }
            else {
                int addr1 = temp_sym_tab[pc.dst];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;

                li("$t0", to_string(temp_sym_focus[pc.op1]));
                if (pc.type == "add") {
                    addi("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                    if (!o3) sw("$t2", addr1, base_reg1);
                } else if (pc.type == "sub") {
                    subi("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                    if (!o3) sw("$t2", addr1, base_reg1);
                } else if (pc.type == "mult") {
                    li("$t1", to_string(temp_sym_focus[pc.op2]));
                    mult("$t0", "$t1");
                    mflo("$t0");
                    if (!o3) sw("$t0", addr1, base_reg1);
                } else if (pc.type == "div") {
                    li("$t1", to_string(temp_sym_focus[pc.op2]));
                    div("$t0", "$t1");
                    mflo("$t0");
                    if (!o3) sw("$t0", addr1, base_reg1);
                }
            }
        }
        else if (pc.type == "neg") {
            int addr1 = temp_sym_tab[pc.dst];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;

            int addr2 = temp_sym_tab[pc.op1];
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg2 = data_base;

            string reg_name0;
            string reg_name1;
            string reg_name_dst;

            if (temp_sym_const[pc.op1] && o1) ;
            else {
                if (temp_sym_const[pc.op1]) {
                    reg_name0 = "$t0";
                    if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
                    else li(reg_name0, to_string(temp_sym_focus[pc.op1]));
                }
                else {
                    reg_name0 = "$t0";
                    if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
                    else lw(reg_name0, addr2, base_reg2);
                }
                reg_name_dst = "$t1";
                if (o3) reg_name_dst = write_allocate_reg(pc.dst);
                neg(reg_name_dst, reg_name0);
                if (!o3) sw(reg_name_dst, addr1, base_reg1);
            }
        }
        else if (pc.type == "lod") {
            string reg_name0;
            string reg_name_dst;
            if (!temp_sym_const[pc.op1]) {
                reg_name0 = "$t0";
                if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
                else if (temp_sym_const[pc.op1]) li(reg_name0, to_string(temp_sym_focus[pc.op1]));
                else lw(reg_name0, temp_sym_tab[pc.op1], data_base);
                reg_name_dst = "$t1";
                if (o3) reg_name_dst = write_allocate_reg(pc.dst);
                string base_reg = "$sp";

                mv(reg_name_dst, reg_name0);
                if (temp_sym_glob[pc.dst]) base_reg = data_base;
                if (pc.op1 != "std::endl") if (!o3) sw(reg_name0, temp_sym_tab[pc.dst], base_reg);
            }
        }
        else if (pc.type == "lod_off") {
            current_top += 4;
            int dst_addr = temp_sym_tab[pc.dst];
            int src_addr = temp_sym_tab[pc.op1];
            int off_addr = temp_sym_tab[pc.op2];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg2 = data_base;
            string base_reg3 = "$sp";
            if (temp_sym_glob[pc.op2]) base_reg3 = data_base;

            string reg_name0;
            string reg_name_dst;

            reg_name0 = "$t0";
            if (o3) reg_name0 = allocate_reg_and_load(pc.op2);
            else if (temp_sym_const[pc.op2]) li(reg_name0, to_string(temp_sym_focus[pc.op2]));
            else lw(reg_name0, off_addr, base_reg3);
            addi("$a2", reg_name0, to_string(src_addr));

            reg_name_dst = "$t1";
            if (o3) reg_name_dst = write_allocate_reg(pc.dst);
            lw(reg_name_dst, "$a2", base_reg2);
            if (!o3) sw(reg_name_dst, dst_addr, base_reg1);
        }
        else if (pc.type == "allocate") {
            current_top += str2int(pc.op2);
            allocate(pc.op1, pc.op2);
        }
        else if (pc.type == "allocate_init") {
            current_top += str2int(pc.op2);
            allocate_init(pc.dst, pc.op1, pc.arr);
        }
        else if (pc.type == "initialize_a") {
            int base_addr = temp_sym_tab[pc.dst];
            for (int i = 0;i < pc.arr.size();i++) {
                li("$a2", to_string(pc.arr[i]));
                sw("$a2", (base_addr+4*i), "$sp");
            }
        }
        else if (pc.type == "li") {
            ;
        }
        else if (pc.type == "const") {
            ;
        }
        else if (pc.type == "var" || pc.type == "para") {
            current_top += str2int(pc.op2);
        }
        else if (pc.type == "print_s") {
            la("$a0", random_key + "string_" + to_string(s_count));
            s_count++;
            li("$v0", "4");
            syscall();
        }
        else if (pc.type == "print_v_int") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = "$a3";
            if (o3 && get_allocated_reg(pc.dst) != "") {
                mv("$a0", get_allocated_reg(pc.dst));
            }
            else if (temp_sym_const[pc.dst]) {
                li("$a0", to_string(temp_sym_focus[pc.dst]));
            }
            else lw("$a0", dst_addr, base_reg);
            li("$v0", "1");
            syscall();
        }
        else if (pc.type == "print_v_char") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = "$a3";
            if (o3 && get_allocated_reg(pc.dst) != "") {
                mv("$a0", get_allocated_reg(pc.dst));
            }
            else if (temp_sym_const[pc.dst]) {
                li("$a0", to_string(temp_sym_focus[pc.dst]));
            }
            else lw("$a0", dst_addr, base_reg);
            li("$v0", "11");
            syscall();
        }
        else if (pc.type == "print_endl") {
            li("$a0", "10");
            li("$v0", "11");
            syscall();
        }
        else if (pc.type == "scanf_int") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = data_base;
            li("$v0", "5");
            syscall();
            if (!o3) sw("$v0", dst_addr, base_reg);
            else {
                string reg_name_dst = write_allocate_reg(pc.dst);
                mv(reg_name_dst, "$v0");
            }
        }
        else if (pc.type == "scanf_char") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = data_base;
            li("$v0", "12");
            syscall();
            if (!o3) sw("$v0", dst_addr, base_reg);
            else {
                string reg_name_dst = write_allocate_reg(pc.dst);
                mv(reg_name_dst, "$v0");
            }
        }
        else if (pc.type == "set_function_flag") {
            cur_func_name = pc.dst;
            current_top = 0;
            if (no_func) {
                set_string();
                set_text_seg();
                li("$a3", "0x10010000");
                subi("$sp", "$sp", to_string(temp_func_tab["main"]));
                j("main");
                no_func = false;
            }
            set_flag(pc.dst);
            sw("$ra", 0, "$sp");
        }
        else if (pc.type == "assign") {
            int dst_addr = temp_sym_tab[pc.dst];
            int src_addr = temp_sym_tab[pc.op1];
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg2 = data_base;
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg1 = data_base;

            string reg_name0;
            string reg_name_dst;

            reg_name0 = "$t0";
            if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
            else if (temp_sym_const[pc.op1]) li(reg_name0, to_string(temp_sym_focus[pc.op1]));
            else lw(reg_name0, src_addr, base_reg1);

            reg_name_dst = "$t1";
            if (o3) reg_name_dst = write_allocate_reg(pc.dst);
            mv(reg_name_dst, reg_name0);
            if (!o3) sw(reg_name_dst, dst_addr, base_reg2);
        }
        else if (pc.type == "assign_off") {
            int dst_addr = temp_sym_tab[pc.dst];
            int src_addr = temp_sym_tab[pc.op1];
            int off_addr = temp_sym_tab[pc.op2];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg2 = data_base;
            string base_reg3 = "$sp";
            if (temp_sym_glob[pc.op2]) base_reg3 = data_base;

            string reg_name0; // offset
            string reg_name1; // value
            string reg_name_dst;

            reg_name0 = "$t0";
            if (o3) reg_name0 = allocate_reg_and_load(pc.op2);
            else if (temp_sym_const[pc.op2]) li(reg_name0, to_string(temp_sym_focus[pc.op2]));
            else lw(reg_name0, off_addr, base_reg3);
            addi("$a2", reg_name0, to_string(dst_addr));

            reg_name1 = "$t1";
            if (o3) reg_name1 = allocate_reg_and_load(pc.op1);
            else if (temp_sym_const[pc.op1]) li(reg_name1, to_string(temp_sym_focus[pc.op1]));
            else lw(reg_name1, src_addr, base_reg2);

            reg_name_dst = "$t2";
            if (o3) reg_name_dst = "$a1";
            mv(reg_name_dst, reg_name1);
            sw(reg_name_dst, "$a2", base_reg1);
        }
        else if (pc.type == "set_label") {
            set_flag(pc.type + "_" + pc.dst);
        }
        else if (pc.type == "beq" || pc.type == "bne" || pc.type == "ble" ||
                 pc.type == "blt" || pc.type == "bge" || pc.type == "bgt") {
            int addr1 = temp_sym_tab[pc.op1];
            int addr2 = temp_sym_tab[pc.op2];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg1 = data_base;
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op2]) base_reg2 = data_base;

            string reg_name0;
            string reg_name1;

            reg_name0 = "$t0";
            if (pc.op1 == "@main_t7") {
                int aaaa = 1;
            }
            if (o3) reg_name0 = allocate_reg_and_load(pc.op1);
            else if (temp_sym_const[pc.op1]) li(reg_name0, to_string(temp_sym_focus[pc.op1]));
            else lw(reg_name0, addr1, base_reg1);

            if (pc.type == "bne" && !temp_sym_const[pc.op1]) sw(reg_name0, addr1, base_reg1);

            if (temp_sym_const[pc.op2]) {
                branch(pc.type, reg_name0, to_string(temp_sym_focus[pc.op2]), pc.dst);
                continue;
            }
            else {
                reg_name1 = "$t1";
                if (o3) reg_name1 = allocate_reg_and_load(pc.op2);
                else lw("$t1", addr2, base_reg2);
            }
            branch(pc.type, reg_name0, reg_name1, pc.dst);

        }
        else if (pc.type == "jump") {
            j(pc.dst);
        }
        else if (pc.type == "set_post_process") {
            int addr1 = temp_sym_tab[pc.dst];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;

            string reg_name0;
            reg_name0 = "$t0";
            if (o3) reg_name0 = allocate_reg_and_load(pc.dst);
            else lw(reg_name0, addr1, base_reg1);
            if (pc.op1 == "0") addi(reg_name0,reg_name0, pc.op2);
            else subi(reg_name0, reg_name0, pc.op2);
            if (!o3) sw(reg_name0, addr1, base_reg1);
        }
        else if (pc.type == "call") {
            over_flow_index = 0;
            save_spot();
            subi("$sp", "$sp", to_string(temp_func_tab[pc.dst] + 4));
            jal(pc.dst);
            int addr1 = temp_sym_tab[pc.op1];
            lw("$ra", 0, "$sp");
            if (pc.op1 != "@None") sw("$v0", addr1, "$sp");
        }
        else if (pc.type == "push") {
            over_flow_index += 4;
            string reg_name0;
            reg_name0 = "$t0";

            if (pc.dst == "@main_t60sub5") {
                int aaaa = 1;
            }

            if (o3) reg_name0 = allocate_reg_and_load(pc.dst);
            else if (temp_sym_const[pc.dst]) li(reg_name0, to_string(temp_sym_focus[pc.dst]));
            else if (temp_sym_glob[pc.dst]) lw(reg_name0, temp_sym_tab[pc.dst], data_base);
            else lw(reg_name0, temp_sym_tab[pc.dst], "$sp");

            sw(reg_name0, over_flow_index - down_move_length - 4, "$sp");
        }
        else if (pc.type == "return") {
            // set RETURN_VAL
            if (pc.dst == "@None") ;
            else if (o3 && get_allocated_reg(pc.dst) != "") mv("$v0", get_allocated_reg(pc.dst));
            else if (temp_sym_const[pc.dst]) li("$v0", to_string(temp_sym_focus[pc.dst]));
            else if (temp_sym_glob[pc.dst]) lw("$v0", temp_sym_tab[pc.dst], data_base);
            else lw("$v0", temp_sym_tab[pc.dst], "$sp");
            // set stack
            addi("$sp", "$sp", to_string(temp_func_tab[cur_func_name] + 4));
            jr("$ra");
        }
        else if (pc.type == "shut down") {
            li("$v0", "10");
            syscall();
        }
        else if (pc.type == "set_down_move") {
            down_move_length = temp_func_tab[pc.dst];
        }
        else if (pc.type == "clear_all") {
            clear_all();
        }
        else if (pc.type == "block_end") {
            write_back_all_t();
        }
        else {
            cout << pc.to_string() << endl;
            cout << "#####FATAL ERROR#####";
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// register pool
/*
 * 使用寄存器流程：
 * 1、程序初始时，要初始化：init_reg_pool()
 * 2、现有一个变量var要加载到寄存器中：
 * if (o) {
 *     bool in_reg = true;
 *     string reg_name = get_allocated_reg(var);
 *     if (reg_name.size() == 0) {
 *         in_reg = false;
 *         reg_name = read_allocate_reg(name);
 *         /////////////////
 *         ///lw，li操作/////
 *         /////////////////
 *     }
 * } else {
 *     /////////////////
 *     ///lw，li操作/////
 *     /////////////////
 * }
 * 3、回写策略
 *    该寄存器被换走，或者函数调用
 */


static string s_regs[8];
static int s_pointer = 0;
static bool s_reserved[8];
static string t_regs[10];
static int t_pointer = 0;
static bool t_reserved[10];
static bool t_use[10];

bool is_temp_var(const string& name) {
    int count = 0;
    for (char i : name) {
        if (i == '@') count++;
    }
    return count == 1;
}

void init_reg_pool() {
    for (int i = 0;i < 8;i++) {
        s_regs[i] = "";
        s_reserved[i] = false;
    }
    for (int i = 0;i < 10;i++) {
        t_regs[i] = "";
        t_reserved[i] = false;
        t_use[i] = false;
    }
}

string get_allocated_reg(const string& name) {
    int i;
    for (i = 0;i < 8;i++) {
        if (s_regs[i] == name) {
            string ans = "$s" + to_string(i);
            s_reserved[i] = true;
            return ans;
        }
    }
    for (i = 0;i < 10;i++) {
        if (t_regs[i] == name) {
            string ans = "$t" + to_string(i);
            t_reserved[i] = true;
            return ans;
        }
    }
    return "";
}

void write_back_all_t() {
    for (t_pointer = 0;t_pointer < 10;t_pointer++) {
        if ((t_regs[t_pointer] != "" && !is_temp_var(t_regs[t_pointer])) || (!t_use[t_pointer] && t_regs[t_pointer] != "")) {
            string write_back_var = t_regs[t_pointer];
            string reg_name = "$t" + to_string(t_pointer);
            string base_reg = "$sp";
            if (temp_sym_glob[write_back_var]) base_reg = data_base;
            if (temp_sym_tab[write_back_var] != 0) {
                sw(reg_name, temp_sym_tab[write_back_var], base_reg);
            }
        }
    }
    t_pointer = 0;
}

void write_back_all_t1() {
    for (t_pointer = 0;t_pointer < 10;t_pointer++) {
        if (t_regs[t_pointer] != "" && !is_temp_var(t_regs[t_pointer])) {
            string write_back_var = t_regs[t_pointer];
            string reg_name = "$t" + to_string(t_pointer);
            string base_reg = "$sp";
            if (temp_sym_glob[write_back_var]) base_reg = data_base;
            if (temp_sym_tab[write_back_var] != 0) {
                sw(reg_name, temp_sym_tab[write_back_var], base_reg);
            }
        }
    }
    t_pointer = 0;
}

void write_back_all_s() {
    for (s_pointer = 0;s_pointer < 8;s_pointer++) {
        if (!s_regs[s_pointer].empty()) {
            string write_back_var = s_regs[s_pointer];
            string reg_name = "$s" + to_string(s_pointer);
            string base_reg = "$sp";
            if (temp_sym_glob[write_back_var]) base_reg = data_base;
            if (!o3) sw(reg_name, temp_sym_tab[reg_name], base_reg);
        }
    }
}

string read_allocate_reg(const string& name, bool for_write) {
    string ans;
    while (t_reserved[t_pointer]) {
        t_pointer = (t_pointer + 1) % 10;
    }
    if (t_regs[t_pointer] != "" && (!t_use[t_pointer] || !is_temp_var(t_regs[t_pointer])) && temp_sym_tab[t_regs[t_pointer]] != 0) {
        string write_back_var = t_regs[t_pointer];
        string reg_name = "$t" + to_string(t_pointer);
        string base_reg = "$sp";
        string out_var = t_regs[t_pointer];
        if (temp_sym_glob[write_back_var]) base_reg = data_base;
        if (temp_sym_tab.find(out_var) != temp_sym_tab.end())
            sw(reg_name, temp_sym_tab[out_var], base_reg);
    }
    if (name != "##TEMP##") t_regs[t_pointer] = name;
    else t_regs[t_pointer] = "";
    t_reserved[t_pointer] = true;

    ans = "$t" + to_string(t_pointer);
    if (for_write) t_use[t_pointer] = false;
    else t_use[t_pointer] = true;
    t_pointer = (t_pointer + 1) % 10;
    return ans;
}

string write_allocate_reg(const string& name, bool replace) {
    string ans;

    for (int i = 0;i < 10;i++) {
        if (t_regs[i] == name) {
            t_use[i] = true;
            return "$t" + to_string(i);
        }
    }

    if (replace) {
        for (int i = 0;i < 10;i++) {
            if (t_reserved[i]) {
                t_use[i] = true;
                return "$t" + to_string(i);
            }
        }
    }
    return read_allocate_reg(name, true);
}

void flush_reservation() {
    for (bool & i : s_reserved) {
        i = false;
    }
    for (bool & i : t_reserved) {
        i = false;
    }
}

string allocate_reg_and_load(const string& var_name) {
    string reg_ans;
    if (get_allocated_reg(var_name) != "") {
        reg_ans = get_allocated_reg(var_name);
        string reg_id = "";
        for (int i = 2;i < reg_ans.size();i++) {
            reg_id += reg_ans[i];
        }
        int id = str2int(reg_id);
        t_use[id] = true;
    }
    else {
        string base_reg = "$sp";
        if (temp_sym_glob[var_name]) base_reg = data_base;
        reg_ans = read_allocate_reg(var_name);
        if (temp_sym_const[var_name]) li(reg_ans, to_string(temp_sym_focus[var_name]));
        else lw(reg_ans, temp_sym_tab[var_name], base_reg);
    }
    return reg_ans;
}

void clear_all() {
    init_reg_pool();
}

void save_spot() {
    write_back_all_t();
    write_back_all_s();
}

#endif //UNTITLED_MIPS_H
