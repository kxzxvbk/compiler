//
// Created by 23207 on 2020/10/22.
//

#ifndef COMPILER_ENTRY_H
#define COMPILER_ENTRY_H
#include <string>
#include <vector>
#include <map>
#include "word_parsing.h"

using namespace std;

// type :
// 0 -> void
// 1 -> int
// 2 -> char

// v_or_f_or_c:
// 1 -> var
// 2 -> func
// 3 -> const

string to_lower(string cc) {
    string ret = cc;
    for (char & i : ret) {
        if ('A' <= i && i <= 'Z') {
            i = i + 32;
        }
    }
    return ret;
}

class Entry {
public:
    Entry(int v_or_f_or_c, string name, int type, int line_n, int value=0) {
        this->v_or_f_or_c = v_or_f_or_c;
        this->name = to_lower(name);
        this->type = type;
        this->line_n = line_n;
        this->value = value;
    }

    string get_name() {
        return this->name;
    }

    int get_value() const {
        return this->value;
    }

    void add_arg(int t) {
        this->args_tab.push_back(t);
    }

    vector<int> get_arg() {
        return this->args_tab;
    }

    void add_shape(int x) {
        this->shape.push_back(x);
    }

    int get_type() const {
        return this->type;
    }

    bool is_const() const {
        return this->v_or_f_or_c == 3;
    }

    vector<int> get_shape() {
        return this->shape;
    }

    int v_or_f_or_c;
private:
    string name;
    int type;
    int line_n;
    int value;

    // func
    vector<int> args_tab;

    // array
    vector<int> shape;
};

class Symbol_tab {
public:
    void append_sym(Entry e) {
        string name11 = e.get_name();
        if (this->has_name(name11)) { this->tab.erase(name11); }
        (this->tab).insert(pair<string, Entry>(e.get_name(), e));
    }

    bool has_name(const string& n) {
        return this->tab.count(to_lower(n)) > 0;
    }

    Entry get(const string& n) {
        map<string, Entry>::iterator iter;
        iter = this->tab.find(to_lower(n));
        return iter->second;
    }
private:
    map<string, Entry> tab;
};

#endif //COMPILER_ENTRY_H
