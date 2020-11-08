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

    string to_string() const {
        if (this->type == "add") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "sub") return dst + "=" + op1 + " " +type + " " + op2;
        if (this->type == "mult") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "multi") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "div") return dst + "=" + op1 + " " + type + " " + op2;
        if (this->type == "lod") return type+ " " + dst+ " " + op1;
        if (this->type == "lod_off") return type + " " + dst + " " + op1 + " offset: " + op2;
        if (this->type == "allocate") return type + " " + dst + " "+ op1+ " " + op2;
        if (this->type == "initialize_a") return type + " " + dst + " " + arr_to_string(arr);
        if (this->type == "li") return type + " " + dst + " " + op1;
        if (this->type == "const") return type + " " + dst + " " + op1;
        if (this->type == "var") return type + " " + dst + " " + op1;
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
        if (this->type == "set_label") return type + " " + op1;
        if (this->type == "end_label") return type + " " + op1;
        if (type == "beq" || type == "bne" || type == "ble" ||
            type == "blt" || type == "bge" || type == "bgt") return type + " " + op1 + " " + op2;
        if (type == "jump") return type + " " + dst;
        cout << "#####FATAL ERROR#####";
        return "";
    }
};

list<Quardcode> qcodes;
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
            string temp_name = "@"+function_name+"_t"+to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name,pc.arr, op1, op2);
            op_stack.push_back(temp_name);
        } else if (pc.type == "lod") {
            t_count++;
            string temp_name = "@"+function_name+"_t"+to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name, pc.arr, pc.op1);
            op_stack.push_back(temp_name);
        } else if (pc.type == "lod_1D") {
            t_count++;
            string temp_name = "@"+function_name+"_t"+to_string(t_count);
            string offset = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back("lod_off", temp_name, pc.arr, pc.op1, offset);
            op_stack.push_back(temp_name);
        } else if (pc.type == "lod_2D") {
            t_count++;
            string temp_name = "@"+function_name+"_t"+to_string(t_count);

            string offset = op_stack.back();
            op_stack.pop_back();

            string d1 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back("multi", d1, pc.arr, d1, pc.op2);
            cout << qcodes.back().to_string() << endl;
            qcodes.emplace_back("add", offset, pc.arr, offset, d1);
            cout << qcodes.back().to_string() << endl;
            qcodes.emplace_back("lod_off", temp_name, pc.arr, pc.op1, offset);
            op_stack.push_back(temp_name);
        } else if (pc.type == "allocate") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2, pc.op3);
        } else if (pc.type == "allocate_init") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2, pc.op3);
        } else if (pc.type == "initialize_a") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        } else if (pc.type == "li") {
            t_count++;
            string temp_name = "@"+function_name+"_t"+to_string(t_count);
            qcodes.emplace_back(pc.type, temp_name, pc.arr, pc.op1);
            op_stack.push_back(temp_name);
        } else if (pc.type == "const") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2);
        } else if (pc.type == "var") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, pc.op2, pc.op3);
        } else if (pc.type == "print_s") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        } else if (pc.type == "print_v_int") {
            string op1 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back(pc.type, op1, pc.arr);
        } else if (pc.type == "print_v_char") {
            string op1 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back(pc.type, op1, pc.arr);
        } else if (pc.type == "scanf_int") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        } else if (pc.type == "scanf_char") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
        } else if (pc.type == "set_function_flag") {
            qcodes.emplace_back(pc.type, pc.op1, pc.arr);
            function_name = pc.op1;
        } else if (pc.type == "assign") {
            string op2 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back(pc.type, pc.op1, pc.arr, op2);
        } else if (pc.type == "assign_1D") {
            string op2 = op_stack.back();
            op_stack.pop_back();
            string offset = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back("assign_off", pc.op1, pc.arr, pc.op2, offset);
        } else if (pc.type == "assign_2D") {
            // answer
            string op2 = op_stack.back();
            op_stack.pop_back();

            // offset
            string offset = op_stack.back();
            op_stack.pop_back();

            string d1 = op_stack.back();
            op_stack.pop_back();
            qcodes.emplace_back("multi", d1, pc.arr, d1, pc.op2);
            cout << qcodes.back().to_string() << endl;
            qcodes.emplace_back("add", offset, pc.arr, offset, d1);
            cout << qcodes.back().to_string() << endl;
            qcodes.emplace_back("assign_off", pc.op1, pc.arr, op2, offset);
        } else if (pc.type == "set_label") {
            qcodes.emplace_back("set_label", pc.op1, pc.arr);
        } else if (pc.type == "end_label") {
            qcodes.emplace_back("set_label", "end_label", pc.arr, pc.op1);
        } else if (pc.type == "branch") {
            string op2 = op_stack.back();
            op_stack.pop_back();

            string op1 = op_stack.back();
            op_stack.pop_back();
            if (pc.op1 == "0") {
                // 小于
                qcodes.emplace_back("bge", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "1") {
                // 小于等于
                qcodes.emplace_back("bgt", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "2") {
                // 大于
                qcodes.emplace_back("ble", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "3") {
                // 大于等于
                qcodes.emplace_back("blt", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "4") {
                // 等于
                qcodes.emplace_back("bne", pc.op2, pc.arr, op1, op2);
            } else if (pc.op1 == "5") {
                // 不等于
                qcodes.emplace_back("beq", pc.op2, pc.arr, op1, op2);
            }
        } else if (pc.type == "jump") {
            qcodes.emplace_back("jump", pc.op1, pc.arr);
        } else if (pc.type == "set_post_process") {
            qcodes.emplace_back("set_post_process", pc.op1, pc.arr, pc.op2, pc.op3);
        } else {
                cout << pc.type << endl;
                cout << "#####FATAL ERROR#####";
            }
        }
        cout << qcodes.back().to_string() << endl;
}

void output_qcodes_to_file() {
    ofstream qout("quard_code.txt");
    for (const auto & qcode : qcodes) {
        qout << qcode.to_string() << endl;
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
