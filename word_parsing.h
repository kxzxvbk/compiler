//
// Created by 23207 on 2020/9/20.
//
#ifndef COMPILER_WORD_PARSING_H
#define COMPILER_WORD_PARSING_H
#include <iostream>
#include <fstream>
#include <memory.h>
#include <list>
#include "symbol.h"
#include "global.h"
using namespace std;

list<symbol> symbols;
list<symbol>::const_iterator list_it;
list<symbol>::const_iterator temp_it;
list<symbol>::const_iterator loc_it;

bool is_alpha(char c) {
    return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_'));
}

string tolower(string cc) {
    string ret = cc;
    for (char & i : ret) {
        if ('A' <= i && i <= 'Z') {
            i = i + 32;
        }
    }
    return ret;
}

void parse_symbols()
{
    fstream f;
    f.open("testfile.txt");
    char c = f.get();
    int line_num = 0;
    int col_num = 0;

    while (true) {
        while (c == ' ') { c = f.get(); }
        if (c == EOF) { break; }
        if (c == '\n' || c == '\r') {
            line_num++;
            col_num = 0;
            c = f.get();
            continue;
        }
        // 字符
        if (c == '\'') {
            string cc;
            // TODO exception
            while ((c = f.get()) != '\'') {
                col_num++;
                cc.append(1, c);
            }
            col_num++;
            c = f.get();
            symbols.emplace_back(2, cc[0], cc, line_num, col_num);
        }
            // 字符串
        else if (c == '"') {
            string cc;
            // TODO exception
            while ((c = f.get()) != '"') {
                col_num++;
                cc.append(1, c);
            }
            col_num++;
            c = f.get();
            symbols.emplace_back(3, cc[0], cc, line_num, col_num);
        }
            // 标识符及保留字
        else if (is_alpha(c)) {
            string cc;
            do {
                cc.append(1, c);
                col_num++;
                c = f.get();
            } while (('0' <= c && c <= '9') || is_alpha(c));

            int k;
            for (k = 0;k < rwn;k++) {
                string ret = tolower(cc);
                if (ret == reserved_words[k]) break;
            }
            if (k < rwn) {
                symbols.emplace_back(reserved2id[k], 0, cc, line_num, col_num);
            } else {
                symbols.emplace_back(0, 0, cc, line_num, col_num);
            }
        }
            // 整数常量
        else if ('0' <= c && c <= '9') {
            int k = 0;
            int num = 0;
            string cc;
            do {
                cc.append(1, c);
                num = 10 * num + (c - '0');
                k = k + 1;
                col_num++;
                c = f.get();
            } while (c >= '0' && c <= '9');

            symbols.emplace_back(1, num, cc, line_num, col_num);
        }
            // +
        else if (c == '+') {
            string cc("+");
            col_num++;
            c = f.get();
            symbols.emplace_back(19, 0, cc, line_num, col_num);
        }
            // -
        else if (c == '-') {
            string cc("-");
            col_num++;
            c = f.get();
            symbols.emplace_back(20, 0, cc, line_num, col_num);
        }
            // *
        else if (c == '*') {
            string cc("*");
            col_num++;
            c = f.get();
            symbols.emplace_back(21, 0, cc, line_num, col_num);
        }
            // /
        else if (c == '/') {
            string cc("/");
            col_num++;
            c = f.get();
            symbols.emplace_back(22, 0, cc, line_num, col_num);
        }
            // 小于等于、小于
        else if (c == '<') {
            col_num++;
            c = f.get();
            if (c == '=') {
                col_num++;
                c = f.get();
                string cc("<=");
                symbols.emplace_back(24, 0, cc, line_num, col_num);
            } else {
                string cc("<");
                symbols.emplace_back(23, 0, cc, line_num, col_num);
            }
        }
            // 大于等于、大于
        else if (c == '>') {
            col_num++;
            c = f.get();
            if (c == '=') {
                col_num++;
                c = f.get();
                string cc(">=");
                symbols.emplace_back(26, 0, cc, line_num, col_num);
            } else {
                string cc(">");
                symbols.emplace_back(25, 0, cc, line_num, col_num);
            }
        }

            // 等于、赋值
        else if (c == '=') {
            col_num++;
            c = f.get();
            if (c == '=') {
                col_num++;
                c = f.get();
                string cc("==");
                symbols.emplace_back(27, 0, cc, line_num, col_num);
            } else {
                string cc("=");
                symbols.emplace_back(30, 0, cc, line_num, col_num);
            }
        }
            // 不等于
        else if (c == '!') {
            col_num++;
            c = f.get();
            if (c == '=') {
                col_num++;
                c = f.get();
                string cc("!=");
                symbols.emplace_back(28, 0, cc, line_num, col_num);
            }
            // TODO exception
        }
            // 冒号
        else if (c == ':') {
            string cc(":");
            col_num++;
            c = f.get();
            symbols.emplace_back(29, 0, cc, line_num, col_num);
        }
            // 分号
        else if (c == ';') {
            string cc(";");
            col_num++;
            c = f.get();
            symbols.emplace_back(31, 0, cc, line_num, col_num);
        }
            // 逗号
        else if (c == ',') {
            string cc(",");
            col_num++;
            c = f.get();
            symbols.emplace_back(32, 0, cc, line_num, col_num);
        }
            // 各种括号
        else if (c == '(') {
            string cc("(");
            col_num++;
            c = f.get();
            symbols.emplace_back(33, 0, cc, line_num, col_num);
        }
        else if (c == ')') {
            string cc(")");
            col_num++;
            c = f.get();
            symbols.emplace_back(34, 0, cc, line_num, col_num);
        }
        else if (c == '[') {
            string cc("[");
            col_num++;
            c = f.get();
            symbols.emplace_back(35, 0, cc, line_num, col_num);
        }
        else if (c == ']') {
            string cc("]");
            col_num++;
            c = f.get();
            symbols.emplace_back(36, 0, cc, line_num, col_num);
        }
        else if (c == '{') {
            string cc("{");
            col_num++;
            c = f.get();
            symbols.emplace_back(37, 0, cc, line_num, col_num);
        }
        else if (c == '}') {
            string cc("}");
            col_num++;
            c = f.get();
            symbols.emplace_back(38, 0, cc, line_num, col_num);
        }

        // TODO exceptions
        else {
            col_num++;
            c = f.get();
        }
    }
    list_it = symbols.cbegin();
}

symbol getsym() {
    symbol ans = *list_it;
    if (t3) {
        if (list_it != symbols.cbegin()) {
            fout << symbol_type[(*temp_it).get_type()] << " ";
            fout << (*temp_it).get_source() << endl;
        }
    }
    temp_it = list_it;
    list_it++;
    return ans;
}

symbol get_loc_sym() {
    symbol ans = *loc_it;
    loc_it++;
    return ans;
}

void output_symbols_to_file() {
    for (auto it = symbols.cbegin(); it != symbols.end(); it++) {
        fout << symbol_type[it->get_type()] << " ";
        fout << it->get_source() << endl;
    }
}

void output_symbols_to_console() {
    while (list_it != symbols.end()) {
        symbol temp = getsym();
        cout << symbol_type[temp.get_type()] << " ";
        cout << temp.get_source() << endl;
    }
}

# endif