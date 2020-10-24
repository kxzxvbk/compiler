//
// Created by 23207 on 2020/10/1.
//

#ifndef COMPILER_GRAMMAR_H
#define COMPILER_GRAMMAR_H
#include "word_parsing.h"
#include "global_error.h"
#include "global.h"
#include <vector>

static list<string> func_name_with_ret;
static list<string> func_name_without_ret;

static vector<Symbol_tab*> symbol_stack;
static Symbol_tab cur_tab;
static int cur_switch_type;
static Entry cur_func(1, "", 1, 1);
static map<string, Symbol_tab> tabs;
static Entry call_func(1, "", 1, 1);
static bool has_ret = false;

int g_expr();
void g_func_call_with_ret();
int g_term();
int g_factor();
void g_programme();
void g_const_declare();
void g_var_declare();
void g_const_def();
void g_var_define();
void g_val_args_tab();
void g_complex_sentence();
void g_loop_sentence();
void g_case_sentence();
void g_assign_sentence();
void g_read();
void g_write();
void g_ret_sentence();
void g_condition_sentence();
void g_sentence_seq();
void g_func_def_with_ret();
void g_func_def_without_ret();
void g_main_func();
void g_var_define_init();
void g_var_define_uninit();
void g_args_tab();
void g_unsigned_int();
void g_integer();
void g_step_length();
void g_sentence();
void g_func_call_without_ret();

bool is_integer() {
    loc_it = list_it;
    symbol sym_loc = sym;
    if (sym_loc.get_type() == 19 || sym_loc.get_type() == 20) sym_loc = get_loc_sym();
    return (sym_loc.get_type() == 1);
}

bool is_func_with_ret(const string& name) {
    string name1 = tolower(name);
    for (const auto & it : func_name_with_ret) {
        if (it == name1) return true;
    }
    return false;
}

bool is_func_without_ret(const string& name) {
    string name1 = tolower(name);
    for (const auto & it : func_name_without_ret) {
        if (it == name1) return true;
    }
    return false;
}

bool is_func_call() {
    loc_it = list_it;
    symbol sym_loc = sym;
    if (sym_loc.get_type() != 0) return false;
    sym_loc = get_loc_sym();
    if (sym_loc.get_type() != 33) return false;
    return true;
}

bool is_assign_sentence() {
    if (sym.get_type() != 0) return false;
    if (global_sym_tab.has_name(sym.get_source())) {
        return global_sym_tab.get(sym.get_source()).v_or_f_or_c != 2;
    }
    return true;
}

bool is_init() {
    loc_it = list_it;
    symbol sym_loc = sym;
    while (sym_loc.get_type() != 31 && sym_loc.get_type() != 32) {
        if (sym_loc.get_type() == 30) return true;
        sym_loc = get_loc_sym();
    }
    return false;
}

bool is_func_declare() {
    loc_it = list_it;
    symbol sym_loc = sym;
    if (sym_loc.get_type() != 5 && sym_loc.get_type() != 6 && sym_loc.get_type() != 7) return false;

    sym_loc = get_loc_sym();
    if (sym_loc.get_type() != 0) return false;

    string name = sym_loc.get_source();
    sym_loc = get_loc_sym();

    if (sym_loc.get_type() != 33) return false;
    return true;
}

bool is_main_declare() {
    loc_it = list_it;
    symbol sym_loc = sym;
    if (sym_loc.get_type() != 7) return false;
    sym_loc = get_loc_sym();
    if (sym_loc.get_type() != 8) return false;
    sym_loc = get_loc_sym();
    if (sym_loc.get_type() != 33) return false;
    sym_loc = get_loc_sym();
    if (sym_loc.get_type() != 34) return false;
    sym_loc = get_loc_sym();
    return true;
}

void g_string() {
    if (sym.get_type() != 3) g_error();
    sym = getsym();
    if (t3) fout << "<字符串>" << endl;
}

void g_integer() {
    if (sym.get_type() == 19 || sym.get_type() == 20) sym = getsym();
    g_unsigned_int();
    if (t3) fout << "<整数>" << endl;
}

void g_relation_op() {
    // 关系运算符
    if (!(sym.get_type() == 23 || sym.get_type() == 24 || sym.get_type() == 25 ||
    sym.get_type() == 26 || sym.get_type() == 27 || sym.get_type() == 28)) {
        g_error();
    }
    sym = getsym();
}

void g_programme() {
    symbol_stack.push_back(&global_sym_tab);
    tabs["@_global"] = global_sym_tab;
    cur_tab = global_sym_tab;
    if (sym.get_type() == 4) g_const_declare();
    if (!(is_func_declare() || is_main_declare())) {
        g_var_declare();
    }
    tabs["@_global"] = cur_tab;
    global_sym_tab = tabs["@_global"];
    while (!is_main_declare()) {
        if (sym.get_type() == 5 || sym.get_type() == 6) g_func_def_with_ret();
        else if (sym.get_type() == 7) g_func_def_without_ret();
        else g_error();
    }
    g_main_func();
    if (t3) fout << "<程序>";
}

void g_const_declare() {
    // 常量说明
    if (sym.get_type() != 4) g_error();
    sym = getsym();
    g_const_def();
    if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
    else sym = getsym();
    while (sym.get_type() == 4) {
        sym = getsym();
        g_const_def();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    }
    if (t3) fout << "<常量说明>" << endl;
}

void g_const_def() {
    // 常量定义
    if (sym.get_type() == 5) {
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (cur_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'b');
        cur_tab.append_sym(Entry(3, sym.get_source(), 1, sym.get_line_n()));
        sym = getsym();
        if (sym.get_type() != 30) g_error();
        sym = getsym();
        g_integer();

        while (sym.get_type() == 32) {
            sym = getsym();
            if (sym.get_type() != 0) g_error();
            if (cur_tab.has_name(sym.get_source()))
                g_error(sym.get_line_n(), 'b');
            cur_tab.append_sym(Entry(3, sym.get_source(), 1, sym.get_line_n()));
            sym = getsym();
            if (sym.get_type() != 30) g_error();
            sym = getsym();
            g_integer();
        }
    } else if (sym.get_type() == 6) {
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (cur_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'b');
        cur_tab.append_sym(Entry(3, sym.get_source(), 2, sym.get_line_n()));
        sym = getsym();
        if (sym.get_type() != 30) g_error();
        sym = getsym();
        if (sym.get_type() != 2) g_error();
        sym = getsym();

        while (sym.get_type() == 32) {
            sym = getsym();
            if (sym.get_type() != 0) g_error();
            if (cur_tab.has_name(sym.get_source()))
                g_error(sym.get_line_n(), 'b');
            cur_tab.append_sym(Entry(3, sym.get_source(), 2, sym.get_line_n()));
            sym = getsym();
            if (sym.get_type() != 30) g_error();
            sym = getsym();
            if (sym.get_type() != 2) g_error();
            sym = getsym();
        }
    } else {
        g_error();
    }
    if (t3) fout << "<常量定义>" << endl;
}

void g_unsigned_int() {
    if (sym.get_type() != 1) g_error();
    sym = getsym();
    if (t3) fout << "<无符号整数>" << endl;
}

void g_declare_head() {
    // 声明头部
    if (sym.get_type() == 5 || sym.get_type() == 6) {
        int type = sym.get_type() - 4;
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (cur_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'b');
        cur_func = Entry(2, sym.get_source(), type, sym.get_line_n());
        cur_tab.append_sym(cur_func);
        tabs["@_global"] = cur_tab;
        global_sym_tab = tabs["@_global"];
        cur_tab = Symbol_tab();
        func_name_with_ret.push_back(tolower(sym.get_source()));
        sym = getsym();
    } else {
        g_error();
    }
    if (t3) fout << "<声明头部>" << endl;
}

int g_const() {
    // 常量
    if (sym.get_type() == 2) {
        sym = getsym();
        if (t3) fout << "<常量>" << endl;
        return 2;
    }
    g_integer();
    if (t3) fout << "<常量>" << endl;
    return 1;
}

void g_var_declare() {
    // 变量说明
    g_var_define();
    if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
    else sym = getsym();
    while ((sym.get_type() == 5 || sym.get_type() == 6) && !is_func_declare()) {
        g_var_define();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    }
    if (t3) fout << "<变量说明>" << endl;
}

void g_var_define() {
    // 变量定义
    if (is_init()) g_var_define_init();
    else g_var_define_uninit();
    if (t3) fout << "<变量定义>" << endl;
}

void g_var_define_uninit(){
    // 无初始化定义
    if (sym.get_type() != 5 && sym.get_type() != 6) g_error();
    int type = sym.get_type() - 4;
    sym = getsym();
    if (sym.get_type() != 0) g_error();
    if (cur_tab.has_name(sym.get_source()))
        g_error(sym.get_line_n(), 'b');
    Entry cur_var = Entry(1, sym.get_source(), type, sym.get_line_n());
    cur_tab.append_sym(cur_var);
    sym = getsym();
    // brackets
    int x = -1, y = -1;
    if (sym.get_type() == 35) {
        sym = getsym();
        x = sym.get_value();
        cur_var.add_shape(x);
        g_unsigned_int();
        if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
        else sym = getsym();
        if (sym.get_type() == 35) {
            sym = getsym();
            y = sym.get_value();
            cur_var.add_shape(y);
            g_unsigned_int();
            if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
            else sym = getsym();
        }
    }

    while (sym.get_type() == 32) {
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (cur_tab.has_name(sym.get_source())) g_error(sym.get_line_n(), 'b');
        cur_var = Entry(1, sym.get_source(), type, sym.get_line_n());
        cur_tab.append_sym(cur_var);
        sym = getsym();
        // brackets
        if (sym.get_type() == 35) {
            sym = getsym();
            x = sym.get_value();
            cur_var.add_shape(x);
            g_unsigned_int();
            if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
            else sym = getsym();
            if (sym.get_type() == 35) {
                sym = getsym();
                y = sym.get_value();
                cur_var.add_shape(y);
                g_unsigned_int();
                if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
                else sym = getsym();
            }
        }
    }
    if (t3) fout << "<变量定义无初始化>" << endl;
}

void g_var_define_init() {
    // 变量定义及初始化
    if (sym.get_type() != 5 && sym.get_type() != 6) g_error();
    int type = sym.get_type() - 4;
    sym = getsym();
    if (sym.get_type() != 0) g_error();
    if (cur_tab.has_name(sym.get_source()))
        g_error(sym.get_line_n(), 'b');
    Entry cur_var = Entry(1, sym.get_source(), type, sym.get_line_n());
    cur_tab.append_sym(cur_var);
    sym = getsym();

    // brackets
    int x = -1, y = -1;
    if (sym.get_type() == 35) {
        sym = getsym();
        x = sym.get_value();
        cur_var.add_shape(x);
        g_unsigned_int();
        if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
        else sym = getsym();
        if (sym.get_type() == 35) {
            sym = getsym();
            y = sym.get_value();
            cur_var.add_shape(y);
            g_unsigned_int();
            if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
            else sym = getsym();
            // 2-D array
            // =
            if (sym.get_type() != 30) g_error();
            sym = getsym();
            int i = 0, j = 0;
            if (sym.get_type() != 37) g_error();
            sym = getsym();
            if (sym.get_type() != 37) g_error();
            sym = getsym();
            int a_type = g_const();
            if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
            j++;
            while (sym.get_type() == 32) {
                sym = getsym();
                a_type = g_const();
                if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
                j++;
            }
            if (sym.get_type() != 38) g_error();
            if (j != y) g_error(sym.get_line_n(), 'n');
            sym = getsym();
            i++;
            j = 0;
            while (sym.get_type() == 32) {
                sym = getsym();
                if (sym.get_type() != 37) g_error();
                sym = getsym();
                a_type = g_const();
                if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
                j++;
                while (sym.get_type() == 32) {
                    sym = getsym();
                    a_type = g_const();
                    if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
                    j++;
                }
                if (sym.get_type() != 38) g_error();
                if (j != y) g_error(sym.get_line_n(), 'n');
                sym = getsym();
                i++;
                j = 0;
            }
            if (sym.get_type() != 38) g_error();
            if (i != x) g_error(sym.get_line_n(), 'n');
            sym = getsym();
        } else {  // 1-D array
            if (sym.get_type() != 30) g_error();
            sym = getsym();
            if (sym.get_type() != 37) g_error();
            sym = getsym();
            int i = 0;
            int a_type = g_const();
            if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
            i++;
            while (sym.get_type() == 32) {
                sym = getsym();
                a_type = g_const();
                if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
                i++;
            }
            if (sym.get_type() != 38) g_error();
            if (i != x) g_error(sym.get_line_n(), 'n');
            sym = getsym();
        }
    } else { // single var
        if (sym.get_type() != 30) g_error();
        sym = getsym();
        int a_type = g_const();
        if (a_type != cur_var.get_type()) g_error(sym.get_line_n(), 'o');
    }
    if (t3) fout << "<变量定义及初始化>" << endl;
}

void g_func_def_with_ret() {
    // 有返回值函数定义
    g_declare_head();
    has_ret = false;
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    g_args_tab();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (sym.get_type() != 37) g_error();
    sym = getsym();
    g_complex_sentence();
    if (sym.get_type() != 38) g_error();
    if (!has_ret) g_error(sym.get_line_n(), 'h');
    sym = getsym();
    tabs["@_global"].append_sym(cur_func);
    tabs[cur_func.get_name()] = cur_tab;
    cur_tab = tabs["@_global"];
    if (t3) fout << "<有返回值函数定义>" << endl;
}

void g_func_def_without_ret() {
    // 无返回值函数
    if (sym.get_type() != 7) g_error();
    sym = getsym();
    if (sym.get_type() != 0) g_error();
    if (cur_tab.has_name(sym.get_source()))
        g_error(sym.get_line_n(), 'b');
    cur_func = Entry(2, sym.get_source(), 0, sym.get_line_n());
    cur_tab.append_sym(cur_func);
    tabs["@_global"] = cur_tab;
    cur_tab = Symbol_tab();
    func_name_without_ret.push_back(tolower(sym.get_source()));
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    g_args_tab();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (sym.get_type() != 37) g_error();
    sym = getsym();
    g_complex_sentence();
    if (sym.get_type() != 38) g_error();
    sym = getsym();
    tabs[cur_func.get_name()] = cur_tab;
    tabs["@_global"].append_sym(cur_func);
    cur_tab = tabs["@_global"];
    global_sym_tab = tabs["@_global"];
    if (t3) fout << "<无返回值函数定义>" << endl;
}

void g_complex_sentence() {
    // 复合语句
    if (sym.get_type() == 4) g_const_declare();
    if (sym.get_type() == 5 || sym.get_type() == 6) g_var_declare();
    g_sentence_seq();
    if (t3) fout << "<复合语句>" << endl;
}

void g_args_tab() {
    // 参数表
    if (sym.get_type() == 5 || sym.get_type() == 6) {
        int type = sym.get_type() - 4;
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        cur_func.add_arg(type);
        if (cur_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'b');
        cur_tab.append_sym(Entry(1, sym.get_source(), type, sym.get_line_n()));

        sym = getsym();
        while (sym.get_type() == 32) {
            sym = getsym();
            if (!(sym.get_type() == 5 || sym.get_type() == 6)) g_error();
            type = sym.get_type() - 4;
            sym = getsym();
            if (sym.get_type() != 0) g_error();
            if (cur_tab.has_name(sym.get_source()))
                g_error(sym.get_line_n(), 'b');
            cur_tab.append_sym(Entry(1, sym.get_source(), type, sym.get_line_n()));
            cur_func.add_arg(type);
            sym = getsym();
        }
    }
    global_sym_tab.append_sym(cur_func);
    if (t3) fout << "<参数表>" << endl;
}

void g_main_func() {
    if (sym.get_type() != 7) g_error();
    sym = getsym();
    if (sym.get_type() != 8) g_error();
    if (cur_tab.has_name(sym.get_source()))
        g_error(sym.get_line_n(), 'b');
    cur_func = Entry(1, "main", 0, sym.get_line_n());
    cur_tab.append_sym(cur_func);
    tabs["@_global"] = cur_tab;
    cur_tab = Symbol_tab();
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (sym.get_type() != 37) g_error();
    sym = getsym();
    g_complex_sentence();
    if (sym.get_type() != 38) g_error();
    tabs[cur_func.get_name()] = cur_tab;
    cur_tab = tabs["@_global"];
    if (t3) fout << symbol_type[sym.get_type()] << " ";
    if (t3) fout << sym.get_source() << endl;
    if (t3) fout << "<主函数>" << endl;
}

int g_factor() {
    // 因子
    int type;
    if (sym.get_type() == 2) {
        sym = getsym();
        type = 2;
    }
    else if (is_integer()) {
        g_integer();
        type = 1;
    }
    else if (sym.get_type() == 33) {
        sym = getsym();
        type = g_expr();
        if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
        else sym = getsym();
    } else if (sym.get_type() == 0) {
        if (!cur_tab.has_name(sym.get_source()) && !global_sym_tab.has_name(sym.get_source())) {
            g_error(sym.get_line_n(), 'c');
            sym = getsym();
            return 3;
        }
        if (is_func_call()) {
            type = (cur_tab.has_name(sym.get_source()))?cur_tab.get(sym.get_source()).get_type() :
                   (global_sym_tab.has_name(sym.get_source()))?global_sym_tab.get(sym.get_source()).get_type():
                   3;
            g_func_call_with_ret();
            if (t3) fout << "<因子>" << endl;
            return type;
        }
        type = (cur_tab.has_name(sym.get_source()))?cur_tab.get(sym.get_source()).get_type() :
               (global_sym_tab.has_name(sym.get_source()))?global_sym_tab.get(sym.get_source()).get_type():
               3;
        sym = getsym();
        // brackets
        if (sym.get_type() == 35) {
            sym = getsym();
            int type1 = g_expr();
            if (type1 != 1) g_error(sym.get_line_n(), 'i');
            if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
            else sym = getsym();
            if (sym.get_type() == 35) {
                sym = getsym();
                type1 = g_expr();
                if (type1 != 1) g_error(sym.get_line_n(), 'i');
                if (sym.get_type() != 36) g_error((*temp_it).get_line_n(), 'm');
                else sym = getsym();
            }
        }
    } else type = 0;
    if (t3) fout << "<因子>" << endl;
    return type;
}

int g_term() {
    // 项
    int type = g_factor();
    while (sym.get_type() == 21 || sym.get_type() == 22) {
        sym = getsym();
        int temp = g_factor();
        if (type == 2 || (type == 0 && temp != 0)) { type = 1; }
        if (temp == 3) type = 3;
    }
    if (t3) fout << "<项>" << endl;
    return type;
}

int g_expr() {
    // 表达式
    int type = 0;
    if (sym.get_type() == 19 || sym.get_type() == 20) {
        sym = getsym();
    }
    type = g_term();
    while (sym.get_type() == 19 || sym.get_type() == 20) {
        sym = getsym();
        int temp = g_term();
        if (type == 2 || (type == 0 && temp != 0)) type = 1;
        if (temp == 3) type = 3;
    }
    if (t3) fout << "<表达式>" << endl;
    return type;
}

void g_sentence() {
    // 语句
    if (sym.get_type() == 15 || sym.get_type() == 14) {
        g_loop_sentence();
    } else if (sym.get_type() == 9) {
        g_condition_sentence();
    } else if (sym.get_type() == 16) {
        g_read();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    } else if (sym.get_type() == 17) {
        g_write();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    } else if (sym.get_type() == 31) {
        sym = getsym();
    } else if (sym.get_type() == 11) {
        g_case_sentence();
    } else if (sym.get_type() == 37) {
        sym = getsym();
        g_sentence_seq();
        if (sym.get_type() != 38) g_error();
        sym = getsym();
    } else if (sym.get_type() == 18) {
        g_ret_sentence();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    } else if (is_assign_sentence()) {
        g_assign_sentence();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    } else {
        if (is_func_without_ret(sym.get_source())) g_func_call_without_ret();
        else if(is_func_with_ret(sym.get_source())) g_func_call_with_ret();
        else {
            g_error(sym.get_line_n(), 'c');
            while (sym.get_type() != 31) {
                sym = getsym();
            }
        }
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
    }
    if (t3) fout << "<语句>" << endl;
}

void g_assign_sentence() {
    // 赋值语句
    if (sym.get_type() != 0) g_error();
    if (!cur_tab.has_name(sym.get_source()) && !global_sym_tab.has_name(sym.get_source()))
        g_error(sym.get_line_n(), 'c');
    else if (cur_tab.has_name(sym.get_source())) {
        if ((cur_tab.get(sym.get_source())).is_const()) g_error(sym.get_line_n(), 'j');
    } else if (global_sym_tab.has_name(sym.get_source())) {
        Entry ee = global_sym_tab.get(sym.get_source());
        if (ee.is_const()) g_error(sym.get_line_n(), 'j');
    }

    sym = getsym();
    // brackets
    if (sym.get_type() == 35) {
        sym = getsym();
        g_expr();
        if (sym.get_type() != 36) g_error(sym.get_line_n(), 'm');
        else sym = getsym();
        if (sym.get_type() == 35) {
            sym = getsym();
            g_expr();
            if (sym.get_type() != 36) g_error(sym.get_line_n(), 'm');
            else sym = getsym();
        }
    }
    if (sym.get_type() != 30) g_error();
    sym = getsym();
    g_expr();
    if (t3) fout << "<赋值语句>" << endl;
}

void g_condition() {
    // 条件
    int type1 = g_expr();
    g_relation_op();
    int type2 = g_expr();
    if (type1 != 1 || type2 != 1) g_error(sym.get_line_n(), 'f');
    if (t3) fout << "<条件>" << endl;
}

void g_condition_sentence() {
    if (sym.get_type() != 9) g_error();
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    g_condition();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    g_sentence();
    if (sym.get_type() == 10) {
        sym = getsym();
        g_sentence();
    }
    if (t3) fout << "<条件语句>" << endl;
}

void g_step_length() {
    g_unsigned_int();
    if (t3) fout << "<步长>" << endl;
}

void g_loop_sentence() {
    if (sym.get_type() == 14) {
        sym = getsym();
        if (sym.get_type() != 33) g_error();
        sym = getsym();
        g_condition();
        if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
        else sym = getsym();
        g_sentence();
    } else if (sym.get_type() == 15) {
        sym = getsym();
        if (sym.get_type() != 33) g_error();
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (!cur_tab.has_name(sym.get_source()) && !global_sym_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'c');
        sym = getsym();
        if (sym.get_type() != 30) g_error();
        sym = getsym();
        g_expr();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
        g_condition();
        if (sym.get_type() != 31) g_error((*temp_it).get_line_n(), 'k');
        else sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (!cur_tab.has_name(sym.get_source()) && !global_sym_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'c');
        sym = getsym();
        if (sym.get_type() != 30) g_error();
        sym = getsym();
        if (sym.get_type() != 0) g_error();
        if (!cur_tab.has_name(sym.get_source()) && !global_sym_tab.has_name(sym.get_source()))
            g_error(sym.get_line_n(), 'c');
        sym = getsym();
        if (sym.get_type() != 19 && sym.get_type() != 20) g_error();
        sym = getsym();
        g_step_length();
        if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
        else sym = getsym();
        g_sentence();
    } else {
        g_error();
    }
    if (t3) fout << "<循环语句>" << endl;
}

void g_default() {
    if (sym.get_type() != 13) {
        g_error(sym.get_line_n(), 'p');
        return;
    }
    else sym = getsym();
    if (sym.get_type() != 29) g_error();
    sym = getsym();
    g_sentence();
    if (t3) fout << "<缺省>" << endl;
}

void g_case_sub_sentence() {
    if (sym.get_type() != 12) g_error();
    sym = getsym();
    int case_type;
    case_type = g_const();
    if (case_type != cur_switch_type) g_error(sym.get_line_n(), 'o');
    if (sym.get_type() != 29) g_error();
    sym = getsym();
    g_sentence();
    if (t3) fout << "<情况子语句>" << endl;
}

void g_case_table() {
    // 情况表
    g_case_sub_sentence();
    while (sym.get_type() == 12) {
        g_case_sub_sentence();
    }
    if (t3) fout << "<情况表>" << endl;
}

void g_case_sentence() {
    if (sym.get_type() != 11) g_error();
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    cur_switch_type = g_expr();
    if (sym.get_line_n() == 69) {
        int aaa = 0;
    }
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (sym.get_type() != 37) g_error();
    sym = getsym();
    g_case_table();
    g_default();
    if (sym.get_type() != 38) g_error();
    sym = getsym();
    if (t3) fout << "<情况语句>" << endl;
}

void g_val_args_tab() {
    vector<int> tmp;
    vector<int> arg_tab = call_func.get_arg();
    while (sym.get_type() != 34 && sym.get_type() != 31) {
        if (sym.get_type() == 32) {
            sym = getsym();
        }
        tmp.push_back(g_expr());
    }
    if (arg_tab.size() != tmp.size()) g_error(sym.get_line_n(), 'd');
    else {
        for (int i = 0; i < arg_tab.size(); i++) {
            if (arg_tab[i] != tmp[i] && tmp[i] != 3) {
                g_error(sym.get_line_n(), 'e');
                break;
            }
        }
    }
    if (t3) fout << "<值参数表>" << endl;
}

void g_func_call_with_ret() {
    if (sym.get_type() != 0) g_error();
    call_func = global_sym_tab.get(sym.get_source());
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    g_val_args_tab();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (t3) fout << "<有返回值函数调用语句>" << endl;
}

void g_func_call_without_ret() {
    if (sym.get_type() != 0) g_error();
    call_func = global_sym_tab.get(sym.get_source());
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    g_val_args_tab();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (t3) fout << "<无返回值函数调用语句>" << endl;
}

void g_sentence_seq() {
    while (sym.get_type() != 38) {
        g_sentence();
    }
    if (t3) fout << "<语句列>" << endl;
}

void g_read() {
    if (sym.get_type() != 16) g_error();
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();
    if (sym.get_type() != 0) g_error();
    if (!cur_tab.has_name(sym.get_source()) && !global_sym_tab.has_name(sym.get_source())) {
        g_error(sym.get_line_n(), 'c');
    } else {
        if (cur_tab.has_name(sym.get_source())) {
            if (cur_tab.get(sym.get_source()).is_const()) g_error(sym.get_line_n(), 'j');
        } else {
            if (global_sym_tab.get(sym.get_source()).is_const()) g_error(sym.get_line_n(), 'j');
        }
    }
    sym = getsym();
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (t3) fout << "<读语句>" << endl;
}

void g_write() {
    if (sym.get_type() != 17) g_error();
    sym = getsym();
    if (sym.get_type() != 33) g_error();
    sym = getsym();

    if (sym.get_type() == 3) {
        g_string();
        if (sym.get_type() == 32) {
            sym = getsym();
            g_expr();
        }
    } else {
        g_expr();
    }
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (t3) fout << "<写语句>" << endl;
}

void g_ret_sentence() {
    if (sym.get_type() != 18) g_error();
    has_ret = true;
    sym = getsym();
    if (sym.get_type() != 33) {
        // return;
        if (t3) fout << "<返回语句>" << endl;
        if (cur_func.get_type() != 0) g_error(sym.get_line_n(), 'h');
        return;
    }
    sym = getsym();
    bool e_p = false;
    if (sym.get_type() == 34 && cur_func.get_type() != 0) {
        g_error(sym.get_line_n(), 'h');
        return;
    }
    if (cur_func.get_type() == 0) {
        g_error(sym.get_line_n(), 'g');
        e_p = true;
    }
    int ret_type = g_expr();
    if (ret_type != cur_func.get_type() && !e_p) g_error(sym.get_line_n(), 'h');
    if (sym.get_type() != 34) g_error((*temp_it).get_line_n(), 'l');
    else sym = getsym();
    if (t3) fout << "<返回语句>" << endl;
}

#endif //COMPILER_GRAMMAR_H
