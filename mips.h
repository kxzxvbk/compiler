//
// Created by 23207 on 2020/11/4.
//

/*
 * when func_return:
 *    set $v0
 *    jr $ra
 *
 * when entering a function:
 *    sw $ra, -4($sp)
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

map<string, int> temp_sym_tab;
map<string ,bool> temp_sym_glob;
map<string, bool> temp_sym_const;
map<string, int> temp_sym_focus;
int top_index = 0;

int str2int(string in) {
    stringstream ss;
    ss << in;
    int size;
    ss >> size;
    return size;
}

void push_tab(const string& name, int size, bool is_global) {
    top_index += size;
    temp_sym_tab[name] = top_index;
    temp_sym_glob[name] = is_global;
    temp_sym_const[name] = false;
}

void push_quot(const string& new_name, const string& old_name, bool is_global) {
    temp_sym_tab[new_name] = temp_sym_tab[old_name];
    temp_sym_glob[new_name] = is_global;
    temp_sym_const[name] = false;
}

void push_const(string name, int value) {
    temp_sym_const[name] = true;
    temp_sym_focus[name] = value;
}

void set_temp_sym_tab() {
    list<Quardcode>::const_iterator iterator;
    bool is_glob = true;
    for (iterator = qcodes.cbegin(); iterator != qcodes.cend(); iterator++) {
        Quardcode pc = *iterator;
        if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "div") {
            push_tab(pc.dst, 4, is_glob);
        } else if (pc.type == "li") {
            push_const(pc.dst, str2int(pc.op1));
        } else if (pc.type == "var" || pc.type == "allocate") {
            int size = str2int(pc.op2);
            push_tab(pc.op1, size, is_glob);
        } else if (pc.type == "set_function_flag") {
            top_index = 0;
            is_glob = false;
        } else if (pc.type == "lod") {
            push_quot(pc.dst, pc.op1, is_glob);
        }
    }
}

string data_base = "$0";

void qcodes2mips() {
    set_temp_sym_tab();
    list<Quardcode>::const_iterator iterator;
    bool no_func = true;
    set_data_seg();

    for (iterator = qcodes.cbegin(); iterator != qcodes.cend(); iterator++) {
        Quardcode pc = *iterator;
        mout << "# " + pc.to_string() << endl;
        if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "div") {
            if (!temp_sym_const[pc.op2]) {
                int addr1 = temp_sym_tab[pc.dst];
                int addr2 = temp_sym_tab[pc.op1];
                int addr3 = temp_sym_tab[pc.op2];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
                string base_reg2 = "$sp";
                if (temp_sym_glob[pc.op1]) base_reg2 = data_base;
                string base_reg3 = "$sp";
                if (temp_sym_glob[pc.op2]) base_reg3 = data_base;

                lw("$t0", addr2, base_reg2);
                lw("$t1", addr3, base_reg3);

                if (pc.type == "add") {
                    add("$t2", "$t0", "$t1");
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "sub") {
                    sub("$t2", "$t0", "$t1");
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "mult") {
                    mult("$t0", "$t1");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
                } else if (pc.type == "div") {
                    div("$t0", "$t1");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
                }
            } else {
                int addr1 = temp_sym_tab[pc.dst];
                int addr2 = temp_sym_tab[pc.op1];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
                string base_reg2 = "$sp";
                if (temp_sym_glob[pc.op1]) base_reg2 = data_base;

                lw("$t0", addr2, base_reg2);
                if (pc.type == "add") {
                    addi("$t2", "$t0", to_string(temp_sym_focus[pc.op2])));
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "sub") {
                    subi("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "mult") {
                    mult("$t0", "$t1");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
                } else if (pc.type == "div") {
                    div("$t0", "$t1");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
                }
            }
        } else if (pc.type == "multi") {
            int addr1 = temp_sym_tab[pc.dst];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
            li("$t0", pc.op1);
            mult("$t0", "$t1");
            mflo("$t0");
            sw("$t0", addr1, base_reg1);
        } else if (pc.type == "lod") {
            ;
        } else if (pc.type == "lod_off") {
            int dst_addr = temp_sym_tab[pc.dst];
            int src_addr = temp_sym_tab[pc.op1];
            int off_addr = temp_sym_tab[pc.op2];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg2 = data_base;
            string base_reg3 = "$sp";
            if (temp_sym_glob[pc.op2]) base_reg3 = data_base;

            lw("$t0", off_addr, base_reg3);
            neg("$t0", "$t0");
            addi("$t1", "$t0", to_string(src_addr));
            lw("$t0", "$t1", base_reg2);
            sw("$t0", dst_addr, base_reg1);
        } else if (pc.type == "allocate") {
            allocate(pc.op1, pc.op2);
        } else if (pc.type == "allocate_init") {
            allocate_init(pc.dst, pc.op1, pc.arr);
        } else if (pc.type == "initialize_a") {
            int base_addr = temp_sym_tab[pc.dst];
            for (int i = 0;i < pc.arr.size();i++) {
                li("$t0", to_string(pc.arr[i]));
                sw("$t0", (base_addr-4*i), "$sp");
            }
        } else if (pc.type == "li") {
            li("$t0", pc.op1);
            int dst_addr = temp_sym_tab[pc.dst];
            sw("$t0", dst_addr, "$sp");
        } else if (pc.type == "const") {
            ;
        } else if (pc.type == "var") {
            ;
        } else if (pc.type == "print_s") {
            string dst = pc.dst;
            int temp_top = top_index;
            temp_top++;
            sb("$0", temp_top, "$sp");
            for (int i = 0; i <= dst.size();i++) {
                temp_top++;
                li("$t0", to_string(dst[dst.size() - i]));
                sb("$t0", temp_top, "$sp");
            }
            li("$v0", "4");
            addi("$a0", "$sp", to_string(-temp_top));
            syscall();
        } else if (pc.type == "print_v_int") {
            int dst_addr = temp_sym_tab[pc.dst];
            lw("$a0", dst_addr, "$sp");
            li("$v0", "1");
            syscall();
        } else if (pc.type == "print_v_char") {
            int dst_addr = temp_sym_tab[pc.dst];
            lw("$a0", dst_addr, "$sp");
            li("$v0", "11");
            syscall();
        } else if (pc.type == "scanf_int") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = data_base;
            li("$v0", "5");
            syscall();
            sw("$v0", dst_addr, base_reg);
        } else if (pc.type == "scanf_char") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = data_base;
            li("$v0", "12");
            syscall();
            sw("$v0", dst_addr, base_reg);
        } else if (pc.type == "set_function_flag") {
            if (no_func) {
                set_text_seg();
                j("main");
                no_func = false;
            }
            set_flag(pc.dst);
        } else if (pc.type == "assign") {
            int dst_addr = temp_sym_tab[pc.dst];
            int src_addr = temp_sym_tab[pc.op1];
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg2 = data_base;
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg1 = data_base;

            lw("$t0", src_addr, base_reg1);
            sw("$t0", dst_addr, base_reg2);
        } else if (pc.type == "assign_off") {
            int dst_addr = temp_sym_tab[pc.dst];
            int src_addr = temp_sym_tab[pc.op1];
            int off_addr = temp_sym_tab[pc.op2];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg2 = data_base;
            string base_reg3 = "$sp";
            if (temp_sym_glob[pc.op2]) base_reg3 = data_base;

            lw("$t0", off_addr, base_reg3);
            neg("$t0", "$t0");
            addi("$t1", "$t0", to_string(dst_addr));
            lw("$t0", src_addr, base_reg2);
            sw("$t0", "$t1", base_reg1);
        } else if (pc.type == "set_label") {
            set_flag(pc.type + "_" + pc.op1);
        } else if (pc.type == "beq" || pc.type == "bne" || pc.type == "ble" ||
        pc.type == "blt" || pc.type == "bge" || pc.type == "bgt") {
            int addr1 = temp_sym_tab[pc.op1];
            int addr2 = temp_sym_tab[pc.op2];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg1 = data_base;
            string base_reg2 = "$sp";
            if (temp_sym_glob[pc.op2]) base_reg2 = data_base;
            lw("$t0", addr1, base_reg1);
            lw("$t1", addr2, base_reg2);
            branch(pc.type, "$t0", "t1", pc.dst);
        } else if (pc.type == "jump") {
            j(pc.dst);
        } else if (pc.type == "set_post_process") {
            int addr1 = temp_sym_tab[pc.op1];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.op1]) base_reg1 = data_base;
            lw("$t0", addr1, base_reg1);
            if (pc.op1 == "0") addi("t0", "$t0", pc.op2);
            else subi("$t0", "$t0", pc.op2);
            sw("$t0", addr1, base_reg1);
        } else {
                cout << pc.to_string() << endl;
                cout << "#####FATAL ERROR#####";
            }
        }
}

#endif //UNTITLED_MIPS_H
