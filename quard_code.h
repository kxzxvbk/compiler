//
// Created by 23207 on 2020/11/5.
//

#ifndef UNTITLED_QUARD_CODE_H
#define UNTITLED_QUARD_CODE_H
#include "list"
#include "pcode.h"
#include <iostream>
#include "vector"
#include <fstream>
#include <sstream>
using namespace std;

string arr_to_string(vector<int> arr);

int string2int(string in) {
    stringstream ss;
    ss << in;
    int size;
    ss >> size;
    return size;
}

class Quardcode {
public:
    Quardcode(string type, string dst, vector<int>arr, string op1="", string op2="") {
        this->type = type;
        this->op1 = op1;
        this->op2 = op2;
        this->dst = dst;
        this->arr = arr;
    }
    string type;
    string op1;
    string op2;
    string dst;
    vector<int> arr;

    string get_right1() const {
        string op;
        if (type == "add") op = "+";
        else if (type == "sub") op = "-";
        else if (type == "mult" || type == "multi_rep") op = "*";
        else if (type == "div") op = "/";
        else if (type == "assign") return op1;
        return op1 + op + op2;
    }

    string get_right2() const {
        string op;
        if (type == "add") op = "+";
        else if (type == "sub") op = "-";
        else if (type == "mult" || type == "multi_rep") op = "*";
        else if (type == "div") op = "/";
        else if (type == "assign") return op1;
        else if (type == "lod") return op1;
        else op = "$Zhsic@WH1l0Ve11Ang2h@0cHu$";
        return op2 + op + op1;
    }

    string get_left() const {
        return dst;
    }

    string to_string() const {
        if (this->type == "add") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "sub") return dst + "=" + op1 + " " +type + " " + op2;
        if (this->type == "mult") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "div") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "lod") return type+ " " + dst+ " " + op1;
        if (this->type == "lod_off") return type + " " + dst + " " + op1 + " offset: " + op2;
        if (this->type == "allocate") return type + " " + dst + " "+ op1+ " " + op2;
        if (this->type == "allocate_init") return type + " " + dst + " "+ arr_to_string(arr);
        if (this->type == "initialize_a") return type + " " + dst + " " + arr_to_string(arr);
        if (this->type == "li") return type + " " + dst + " " + op1;
        if (this->type == "const") return type + " " + dst + " " + op1;
        if (this->type == "var") return type + " " + dst + " " + op1 + " " + op2;
        if (this->type == "print_s") {
            if (this->dst == "\n") return type + " \"" + "\"";;
            return type + " \"" + dst + "\"";
        }
        if (this->type == "print_v_int") return type + " " + dst;
        if (this->type == "print_v_char") return type + " " + dst;
        if (this->type == "scanf_int") return type + " " + dst;
        if (this->type == "scanf_char") return type + " " + dst;
        if (this->type == "set_function_flag") return type + " " + dst;
        if (this->type == "assign") return dst + "=" + op1;
        if (this->type == "assign_off") return dst + "=" + op1 + " offset: " + op2;
        if (this->type == "set_label") return type + "_" + dst;
        if (this->type == "set_if_label") return type + " " + op1;
        if (this->type == "end_label") return type + " " + op1;
        if (type == "beq" || type == "bne" || type == "ble" ||
            type == "blt" || type == "bge" || type == "bgt") {
            return type + " " + op1 + " " + op2 + " " + dst;
        }
        if (type == "jump") return type + " " + dst;
        if (type == "push") return type + " " + dst;
        if (type == "para") return type + " " + dst;
        if (type == "return") return type + " " + dst;
        if (type == "call") return type + " " + dst;
        if (type == "pop back") return type;
        if (type == "shut down") return type;
        if (type == "set_post_process") return type + " " + dst + " " + op2;
        if (type == "print_endl") return type;
        if (type == "multi_rep") return dst + "=" + op1 + " " + "*" + " " + op2;
        if (type == "neg") return dst + "=-" + op1;
        if (type == "set_down_move") return type + " " + dst;
        if (type == "clear_all") return type;
        if (type == "block_end") return type;
        cout << type << "#####FATAL ERROR#####";
        return "";
    }
};

list<Quardcode> qcodes;
list<Quardcode> push_stack;

void flush(int size) {
    list<Quardcode>temp_stack;
    for (int i = 0; i < size;i++) {
        Quardcode qq = push_stack.back();
        push_stack.pop_back();
        temp_stack.push_back(qq);
    }

    for (int i = 0; i < size;i++) {
        Quardcode qq = temp_stack.back();
        temp_stack.pop_back();
        qcodes.push_back(qq);
    }

}

void pcode2quardcode() {
    list<Pcode>::const_iterator iterator;
    list<string> op_stack;
    string function_name = "#GLOBAL";
    int t_count = 0;

    for (iterator = pcodes.cbegin(); iterator != pcodes.cend(); iterator++) {
        Pcode pc = *iterator;
        if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "div") {
            t_count++;
            string op2 = op_stack.back();
            op_stack.pop_back();
            string op1 = op_stack.back();
            op_stack.pop_back();
            string temp_name = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name, pc.arr, op1, op2);
            op_stack.push_back(temp_name);
        }
        else if (pc.type == "neg") {
            t_count++;
            string op = op_stack.back();
            op_stack.pop_back();
            string temp_name = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name, pc.arr, op);
            op_stack.push_back(temp_name);
        }
        else if (pc.type == "lod") {
            t_count++;
            string temp_name = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name, pc.arr, pc.op1);
            op_stack.push_back(temp_name);
        }
        else if (pc.type == "lod_1D") {
            t_count++;
            string temp_name = "@" + function_name + "_t" + to_string(t_count);
            t_count++;
            string temp_name1 = "@" + function_name + "_t" + to_string(t_count);
            string offset = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back("multi_rep", temp_name1, pc.arr, offset, "4");
            qcodes.emplace_back("lod_off", temp_name, pc.arr, pc.op1, temp_name1);
            op_stack.push_back(temp_name);
        }
        else if (pc.type == "lod_2D") {
            t_count++;
            string temp_name1 = "@" + function_name + "_t" + to_string(t_count);
            t_count++;
            string temp_name2 = "@" + function_name + "_t" + to_string(t_count);
            t_count++;
            string offset2 = "@" + function_name + "_t" + to_string(t_count);
            t_count++;
            string offset3 = "@" + function_name + "_t" + to_string(t_count);

            string offset = op_stack.back();
            op_stack.pop_back();

            string d1 = op_stack.back();
            op_stack.pop_back();

            int off_int = string2int(pc.op2);
            qcodes.emplace_back("multi_rep", temp_name1, pc.arr, d1, to_string(off_int));
            qcodes.emplace_back("add", offset2, pc.arr, offset, temp_name1);\
            qcodes.emplace_back("multi_rep", offset3, pc.arr, offset2, "4");
            qcodes.emplace_back("lod_off", temp_name2, pc.arr, pc.op1, offset3);
            op_stack.push_back(temp_name2);
        }
        else if (pc.type == "allocate") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "allocate_init") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "initialize_a") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        } else if (pc.type == "li") {
            t_count++;
            string temp_name = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name, pc.arr, pc.op1);
            op_stack.push_back(temp_name);
        }
        else if (pc.type == "const") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2);
        }
        else if (pc.type == "var") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "print_s") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        }
        else if (pc.type == "print_v_int") {
            string op1 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back(pc.type, op1, pc.arr);
        }
        else if (pc.type == "print_endl") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        }
        else if (pc.type == "print_v_char") {
            string op1 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back(pc.type, op1, pc.arr);
        }
        else if (pc.type == "scanf_int") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        }
        else if (pc.type == "scanf_char") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        }
        else if (pc.type == "set_function_flag") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
            function_name = pc.op1;
        }
        else if (pc.type == "assign") {
            string op2 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, op2);
        }
        else if (pc.type == "assign_1D") {
            string op2 = op_stack.back();
            op_stack.pop_back();
            string offset = op_stack.back();
            op_stack.pop_back();
            t_count++;
            string temp_name1 = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back("multi_rep", temp_name1, pc.arr, offset, "4");
            qcodes.emplace_back("assign_off", pc.op1, pc.arr, op2, temp_name1);
        }
        else if (pc.type == "assign_2D") {
            // answer
            string op2 = op_stack.back();
            op_stack.pop_back();

            // offset
            string offset = op_stack.back();
            op_stack.pop_back();

            string d1 = op_stack.back();
            op_stack.pop_back();

            t_count++;
            string temp_name = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back("multi_rep", temp_name, pc.arr, d1, pc.op2);

            t_count++;
            string temp_name1 = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back("add", temp_name1, pc.arr, temp_name, offset);

            t_count++;
            string temp_name2 = "@" + function_name + "_t" + to_string(t_count);
            qcodes.emplace_back("multi_rep", temp_name2, pc.arr, temp_name1, "4");

            qcodes.emplace_back("assign_off", pc.op1, pc.arr, op2, temp_name2);
        }
        else if (pc.type == "set_label") {
            qcodes.emplace_back("set_label", pc.op1, pc.arr);
        }
        else if (pc.type == "end_label") {
            qcodes.emplace_back("set_label", "end_label_" + pc.op1, pc.arr);
        }
        else if (pc.type == "branch" || pc.type == "branch_no_get") {
            string op2 = op_stack.back();
            op_stack.pop_back();

            string op1 = op_stack.back();
            if (pc.type != "branch_no_get") op_stack.pop_back();
            if (pc.op1 == "0") {
                // 小于
                if (pc.op3 == "no_reverse") qcodes.emplace_back("blt", pc.op2, pc.arr, op1, op2);
                else qcodes.emplace_back("bge", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "1") {
                // 小于等于
                if (pc.op3 == "no_reverse") qcodes.emplace_back("ble", pc.op2, pc.arr, op1, op2);
                else qcodes.emplace_back("bgt", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "2") {
                // 大于
                if (pc.op3 == "no_reverse") qcodes.emplace_back("bgt", pc.op2, pc.arr, op1, op2);
                else qcodes.emplace_back("ble", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "3") {
                // 大于等于
                if (pc.op3 == "no_reverse") qcodes.emplace_back("bge", pc.op2, pc.arr, op1, op2);
                else qcodes.emplace_back("blt", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "4") {
                // 等于
                if (pc.op3 == "no_reverse") qcodes.emplace_back("beq", pc.op2, pc.arr, op1, op2);
                else qcodes.emplace_back("bne", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "5") {
                // 不等于
                if (pc.op3 == "no_reverse") qcodes.emplace_back("bne", pc.op2, pc.arr, op1, op2);
                else qcodes.emplace_back("beq", pc.op2, pc.arr, op1, op2);
            }
        }
        else if (pc.type == "jump") {
            qcodes.emplace_back("jump", pc.op1, pc.arr);
        }
        else if (pc.type == "set_post_process") {
            qcodes.emplace_back("set_post_process", pc.op1, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "call") {
            qcodes.emplace_back("set_down_move", pc.op1, pc.arr);
            flush(string2int(pc.op2));
            if (pc.op3 != "no_return") {
                t_count++;
                string temp_name = "@" + function_name + "_t" + to_string(t_count);
                qcodes.emplace_back(pc.type, pc.op1, pc.arr, temp_name);
                op_stack.push_back(temp_name);
            }
            else {
                qcodes.emplace_back(pc.type, pc.op1, pc.arr, "@None");
            }
        }
        else if (pc.type == "para") {
            qcodes.emplace_back("para", pc.op1, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "push") {
            string op = op_stack.back();
            op_stack.pop_back();
            push_stack.emplace_back("push", op, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "return") {
            if (pc.op1 == "@None") qcodes.emplace_back("return", pc.op1, pc.arr, pc.op2, pc.op3);
            else {
                string op = op_stack.back();
                op_stack.pop_back();
                qcodes.emplace_back("return", op, pc.arr, pc.op2, pc.op3);
            }
        }
        else if (pc.type == "pop stack") {
            op_stack.pop_back();
        }
        else if (pc.type == "shut down") {
            qcodes.emplace_back("shut down", pc.op1, pc.arr, pc.op2, pc.op3);
        }
        else if (pc.type == "push_to_opstack") {
            op_stack.emplace_back(pc.op1);
        }
        else {
            cout << pc.type << endl;
            cout << "#####FATAL ERROR#####";
        }
    }
}

void output_qcodes_to_file() {
    ofstream qout("quard_code.txt");
    for (const auto & qcode : qcodes) {
        qout << qcode.to_string() << endl;
    }
}

void output_optimized_qcodes_to_file() {
    ofstream oout("optimized_quard_code.txt");
    for (const auto & qcode : qcodes) {
        oout << qcode.to_string() << endl;
    }
}

string arr_to_string(vector<int> arr) {
    string ans = "[";
    for (int i : arr) {
        ans += to_string(i);
        ans += ", ";
    }
    ans += "]";
    return ans;
}

#endif //UNTITLED_QUARD_CODE_H
