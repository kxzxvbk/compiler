//
// Created by 23207 on 2020/11/21.
//

#ifndef UNTITLED_BASIC_BLOCK_H
#define UNTITLED_BASIC_BLOCK_H
#include "vector"
#include "quard_code.h"

class basic_block{
public:
    basic_block(vector<Quardcode> init) {
        this->content = init;
    }

    void add_descendent(int d) {
        descendents.push_back(d);
    }

    // 局部优化
    bool optimization_step() {
        vector<Quardcode> temp_exprs;
        bool ans = false;
        for (auto & pc : content) {
            // 所有可优化的情况
            if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "lod" ||
                pc.type == "div" || pc.type == "multi_rep" || pc.type == "assign" || pc.type == "neg" ||
                pc.type == "print_v_int" || pc.type == "print_v_char" || pc.type == "push" ||
                pc.type == "assign_off" || pc.type == "lod_off" || pc.type == "li") {
                string cur_right1 = pc.get_right1();
                string cur_right2 = pc.get_right2();
                bool find = false;
                for (auto & pc1 : temp_exprs) {
                    // op1, op2均要考虑且具有交换律
                    if (pc.type == "add" || pc.type == "mult" || pc.type == "multi_rep") {
                        if (cur_right1 == pc1.get_right1() || cur_right2 == pc1.get_right1()) {
                            Quardcode new_one = Quardcode("assign", pc.dst, pc.arr, pc1.get_left());
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                        else if ((pc1.type == "assign" || pc1.type == "lod") && (pc.op1 == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc.dst, pc.arr, pc1.op1, pc.op2);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                        else if ((pc1.type == "assign" || pc1.type == "lod") && (pc.op2 == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc.dst, pc.arr, pc.op1, pc1.op1);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                    }
                    // op1, op2均要考虑且没有交换律
                    else if (pc.type == "sub" || pc.type == "div" || pc.type == "assign_off") {
                        if (cur_right1 == pc1.get_right1()) {
                            Quardcode new_one = Quardcode("assign", pc.dst, pc.arr, pc1.get_left());
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                        else if ((pc1.type == "assign" || pc1.type == "lod") && (pc.op1 == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc.dst, pc.arr, pc1.op1, pc.op2);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                        else if ((pc1.type == "assign" || pc1.type == "lod") && (pc.op2 == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc.dst, pc.arr, pc.op1, pc1.op1);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                    }
                    // 仅考虑op2
                    else if (pc.type == "lod_off") {
                        if ((pc1.type == "assign" || pc1.type == "lod" ) && (pc.op2 == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc.dst, pc.arr, pc.op1, pc1.op1);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                    }
                    // 仅考虑dst
                    else if (pc.type == "print_v_int" || pc.type == "print_v_char" || pc.type == "push") {
                        if ((pc1.type == "assign" || pc1.type == "lod" ) && (pc.dst == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc1.op1, pc.arr, pc.op1, pc.op2);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                    }
                    // 仅考虑op1
                    else if (pc.type == "assign" || pc.type == "lod" || pc.type == "neg") {
                        if ((pc1.type == "assign" || pc1.type == "lod" ) && (pc.op1 == pc1.dst)) {
                            if (!is_temp_var(pc1.dst)) continue;
                            Quardcode new_one = Quardcode(pc.type, pc.dst, pc.arr, pc1.op1, pc.op2);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                    }

                    else if (pc.type == "li") {
                        if (pc1.type == "li" && pc.op1 == pc1.op1) {
                            if (!is_temp_var(pc.dst)) continue;
                            Quardcode new_one = Quardcode("assign", pc.dst, pc1.arr, pc1.dst, pc.op2);
                            temp_exprs.push_back(new_one);
                            find = true;
                            break;
                        }
                    }
                }
                if (!find) temp_exprs.push_back(pc);  // no matching expression
                ans = ans || find;
            }
            else temp_exprs.push_back(pc);  // not expression
        }

        // write back to content
        content.clear();
        for (auto & pc : temp_exprs) {
            content.push_back(pc);
        }
        return ans;
    }

    void optimization() {
        while (optimization_step());
        remove_redundant_temp_var();
    }

    vector<Quardcode> get_content() {
        return content;
    }
private:
    vector<int> descendents;
    vector<Quardcode> content;

    bool is_temp_var(const string& name) {
        int count = 0;
        for (char c : name) {
            if (c == '@') count++;
        }
        return count == 1;
    }

    void remove_redundant_temp_var() {
        vector<Quardcode> temp_buf;
        vector<string> used_names;
        for (int i = content.size() - 1; i >= 0; i--) {
            Quardcode pc = content[i];
            // may be able to delete
            if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" || pc.type == "neg" ||
                pc.type == "div" || pc.type == "multi_rep" || pc.type == "lod_off" ||
                pc.type == "assign_off" || pc.type == "lod" || pc.type == "assign") {
                bool is_used = false;
                for (const string &n : used_names) {
                    if (n == pc.dst) {
                        is_used = true;
                        break;
                    }
                }
                if (!is_used && is_temp_var(pc.dst)) continue;
            }
            // add references to used_name
            if (pc.type == "add" || pc.type == "sub" || pc.type == "mult" ||
                pc.type == "div" || pc.type == "multi_rep" || pc.type == "lod_off" ||
                pc.type == "assign_off") {
                used_names.push_back(pc.op1);
                used_names.push_back(pc.op2);
                temp_buf.push_back(pc);
            }
            else if (pc.type == "assign" || pc.type == "lod" || pc.type == "neg") {
                used_names.push_back(pc.op1);
                temp_buf.push_back(pc);
            }
            else if (pc.type == "print_v_int" || pc.type == "print_v_char" ||
                     pc.type == "push" || pc.type == "return" || pc.type == "set_post_process") {
                used_names.push_back(pc.dst);
                temp_buf.push_back(pc);
            }
            else if (pc.type == "beq" || pc.type == "bne" || pc.type == "ble" ||
                     pc.type == "blt" || pc.type == "bge" || pc.type == "bgt") {
                used_names.push_back(pc.op2);
                used_names.push_back(pc.op1);
                temp_buf.push_back(pc);
            }
            else temp_buf.push_back(pc);
        }

        // write back
        content.clear();
        for (int i = temp_buf.size() - 1;i >= 0;i--) {
            content.push_back(temp_buf[i]);
        }
    }
};

#endif //UNTITLED_BASIC_BLOCK_H
