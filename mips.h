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

void push_func(string name) {
    temp_func_tab[name] = top_index;
}

void push_const(string name, int value) {
    temp_sym_const[name] = true;
    temp_sym_focus[name] = value;
}

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
            if (temp_sym_const[pc.op1] && temp_sym_const[pc.op2] && o1) {
                if (pc.type == "add") push_const(pc.dst, temp_sym_focus[pc.op1] + temp_sym_focus[pc.op2]);
                else if (pc.type == "sub") push_const(pc.dst, temp_sym_focus[pc.op1] - temp_sym_focus[pc.op2]);
                else if (pc.type == "mult") push_const(pc.dst, temp_sym_focus[pc.op1] * temp_sym_focus[pc.op2]);
                else if (pc.type == "div") push_const(pc.dst, temp_sym_focus[pc.op1] / temp_sym_focus[pc.op2]);
            }
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
        mout << "# " + pc.to_string() << endl;
        if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "div"
        || pc.type == "multi_rep") {
            current_top += 4;
            if (pc.type == "multi_rep") {
                int addr = temp_sym_tab[pc.op1];
                string base_reg = "$sp";
                if (temp_sym_glob[pc.op1]) base_reg = data_base;

                int addr1 = temp_sym_tab[pc.dst];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg = data_base;

                if (temp_sym_const[pc.op1] && o1);
                else {
                    if (temp_sym_const[pc.op1]) li("$t0", to_string(temp_sym_focus[pc.op1]));
                    else lw("$t0", addr, base_reg);
                    int con = str2int(pc.op2);
                    if (o1 && get_log(con) > 0) {
                        if (con < 0) neg("$t0", "$t0");
                        sll("$t0", "$t0", to_string(get_log(con)));
                    }
                    else if (pc.op2 != "1" || !o1) {
                        li("$t1", pc.op2);
                        mult("$t0", "$t1");
                        mflo("$t0");
                    }
                    sw("$t0", addr1, base_reg1);
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
            }
            else if (!temp_sym_const[pc.op1]){
                int addr1 = temp_sym_tab[pc.dst];
                int addr2 = temp_sym_tab[pc.op1];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
                string base_reg2 = "$sp";
                if (temp_sym_glob[pc.op1]) base_reg2 = data_base;

                lw("$t0", addr2, base_reg2);
                if (pc.type == "add") {
                    if (temp_sym_focus[pc.op2] == 0 && o1) {
                        sw("$t0", addr1, base_reg1);
                    }
                    else {
                        addi("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                        sw("$t2", addr1, base_reg1);
                    }
                } else if (pc.type == "sub") {
                    if (temp_sym_focus[pc.op2] == 0 && o1) {
                        sw("$t0", addr1, base_reg1);
                    }
                    else {
                        sub("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                        sw("$t2", addr1, base_reg1);
                    }
                } else if (pc.type == "mult") {
                    if (temp_sym_focus[pc.op2] == 1 && o1) {
                        sw("$t0", addr1, base_reg1);
                    }
                    else if (o1 && get_log(temp_sym_focus[pc.op2]) > 0) {
                        if (temp_sym_focus[pc.op2] < 0) neg("$t0", "$t0");
                        sll("$t0", "$t0", to_string(get_log(temp_sym_focus[pc.op2])));
                        sw("$t0", addr1, base_reg1);
                    }
                    else {
                        li("$t1", to_string(temp_sym_focus[pc.op2]));
                        mult("$t0", "$t1");
                        mflo("$t0");
                        sw("$t0", addr1, base_reg1);
                    }
                } else if (pc.type == "div") {
                    if (temp_sym_focus[pc.op2] == 1 && o1) {
                        sw("$t0", addr1, base_reg1);
                    }
                    else if (o1 && get_log(temp_sym_focus[pc.op2]) > 0) {
                        if (temp_sym_focus[pc.op2] < 0) neg("$t0", "$t0");
                        sra("$t0", "$t0", to_string(get_log(temp_sym_focus[pc.op2])));
                        sw("$t0", addr1, base_reg1);
                    }
                    else {
                        li("$t1", to_string(temp_sym_focus[pc.op2]));
                        div("$t0", "$t1");
                        mflo("$t0");
                        sw("$t0", addr1, base_reg1);
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

                lw("$t0", addr2, base_reg2);
                if (pc.type == "add") {
                    if (temp_sym_focus[pc.op1] == 0 && o1) {
                        sw("$t0", addr1, base_reg1);
                    }
                    else {
                        addi("$t2", "$t0", to_string(temp_sym_focus[pc.op1]));
                        sw("$t2", addr1, base_reg1);
                    }
                } else if (pc.type == "sub") {
                    li("$t1", to_string(temp_sym_focus[pc.op1]));
                    sub("$t2", "$t1", "$t0");
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "mult") {
                    if (temp_sym_focus[pc.op1] == 1 && o1) {
                        sw("$t0", addr1, base_reg1);
                    }
                    else if (o1 && get_log(temp_sym_focus[pc.op1]) > 0) {
                        if (temp_sym_focus[pc.op1] < 0) neg("$t0", "$t0");
                        sll("$t0", "$t0", to_string(get_log(temp_sym_focus[pc.op1])));
                        sw("$t0", addr1, base_reg1);
                    }
                    else {
                        li("$t1", to_string(temp_sym_focus[pc.op1]));
                        mult("$t0", "$t1");
                        mflo("$t0");
                        sw("$t0", addr1, base_reg1);
                    }
                } else if (pc.type == "div") {
                    li("$t1", to_string(temp_sym_focus[pc.op1]));
                    div("$t1", "$t0");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
                }
            }
            else if (o1);
            else {
                int addr1 = temp_sym_tab[pc.dst];
                string base_reg1 = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg1 = data_base;

                li("$t0", to_string(temp_sym_focus[pc.op1]));
                if (pc.type == "add") {
                    addi("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "sub") {
                    subi("$t2", "$t0", to_string(temp_sym_focus[pc.op2]));
                    sw("$t2", addr1, base_reg1);
                } else if (pc.type == "mult") {
                    li("$t1", to_string(temp_sym_focus[pc.op2]));
                    mult("$t0", "$t1");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
                } else if (pc.type == "div") {
                    li("$t1", to_string(temp_sym_focus[pc.op2]));
                    div("$t0", "$t1");
                    mflo("$t0");
                    sw("$t0", addr1, base_reg1);
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

            if (temp_sym_const[pc.op1] && o1) ;
            else {
                if (temp_sym_const[pc.op1]) li("$t0", to_string(temp_sym_focus[pc.op1]));
                else lw("$t0", addr2, base_reg2);
                neg("$t1", "$t0");
                sw("$t1", addr1, base_reg1);
            }
        }
        else if (pc.type == "lod") {
            if (!start_with(pc.op1, "@")) {
                if (temp_sym_const[pc.op1]) li("$t0", to_string(temp_sym_focus[pc.op1]));
                else lw("$t0", temp_sym_tab[pc.op1], data_base);
                string base_reg = "$sp";
                if (temp_sym_glob[pc.dst]) base_reg = data_base;
                if (pc.op1 != "std::endl") sw("$t0", temp_sym_tab[pc.dst], base_reg);
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

            if (temp_sym_const[pc.op2]) li("$t0", to_string(temp_sym_focus[pc.op2]));
            else lw("$t0", off_addr, base_reg3);
            addi("$t1", "$t0", to_string(src_addr));
            lw("$t0", "$t1", base_reg2);
            sw("$t0", dst_addr, base_reg1);
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
                li("$t0", to_string(pc.arr[i]));
                sw("$t0", (base_addr+4*i), "$sp");
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
            if (temp_sym_const[pc.dst]) {
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
            if (temp_sym_const[pc.dst]) {
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
            sw("$v0", dst_addr, base_reg);
        }
        else if (pc.type == "scanf_char") {
            int dst_addr = temp_sym_tab[pc.dst];
            string base_reg = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg = data_base;
            li("$v0", "12");
            syscall();
            sw("$v0", dst_addr, base_reg);
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

            if (temp_sym_const[pc.op1]) li("$t0", to_string(temp_sym_focus[pc.op1]));
            else lw("$t0", src_addr, base_reg1);
            sw("$t0", dst_addr, base_reg2);
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

            if (temp_sym_const[pc.op2]) li("$t0", to_string(temp_sym_focus[pc.op2]));
            else lw("$t0", off_addr, base_reg3);
            addi("$t1", "$t0", to_string(dst_addr));
            if (temp_sym_const[pc.op1]) li("$t0", to_string(temp_sym_focus[pc.op1]));
            else lw("$t0", src_addr, base_reg2);
            sw("$t0", "$t1", base_reg1);
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
            if (temp_sym_const[pc.op1]) li("$t0", to_string(temp_sym_focus[pc.op1]));
            else lw("$t0", addr1, base_reg1);
            if (temp_sym_const[pc.op2]) {
                branch(pc.type, "$t0", to_string(temp_sym_focus[pc.op2]), pc.dst);
                continue;
            }
            else lw("$t1", addr2, base_reg2);
            branch(pc.type, "$t0", "$t1", pc.dst);

        }
        else if (pc.type == "jump") {
            j(pc.dst);
        }
        else if (pc.type == "set_post_process") {
            int addr1 = temp_sym_tab[pc.dst];
            string base_reg1 = "$sp";
            if (temp_sym_glob[pc.dst]) base_reg1 = data_base;
            lw("$t0", addr1, base_reg1);
            if (pc.op1 == "0") addi("$t0", "$t0", pc.op2);
            else subi("$t0", "$t0", pc.op2);
            sw("$t0", addr1, base_reg1);
        }
        else if (pc.type == "call") {
            over_flow_index = 0;
            subi("$sp", "$sp", to_string(temp_func_tab[pc.dst] + 4));
            jal(pc.dst);
            int addr1 = temp_sym_tab[pc.op1];
            lw("$ra", 0, "$sp");
            if (pc.op1 != "@None") sw("$v0", addr1, "$sp");
        }
        else if (pc.type == "push") {
            over_flow_index += 4;
            if (temp_sym_const[pc.dst]) li("$t0", to_string(temp_sym_focus[pc.dst]));
            else if (temp_sym_glob[pc.dst]) lw("$t0", temp_sym_tab[pc.dst], data_base);
            else lw("$t0", temp_sym_tab[pc.dst], "$sp");

            sw("$t0", over_flow_index - down_move_length - 4, "$sp");
        }
        else if (pc.type == "return") {
            // set RETURN_VAL
            if (pc.dst == "@None") ;
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
        else {
                cout << pc.to_string() << endl;
                cout << "#####FATAL ERROR#####";
            }
        }
}

#endif //UNTITLED_MIPS_H
