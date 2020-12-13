//
// Created by 23207 on 2020/11/21.
//

#ifndef UNTITLED_FLOW_GRAPH_H
#define UNTITLED_FLOW_GRAPH_H
#include "mips.h"
#include "basic_block.h"


bool is_case_jump(string name) {
    int count = 0;
    for (int i = 0;i < name.size();i++) {
        if (name[i] == '_') count++;
    }
    return count == 3;

}

class flow_graph{
public:
    flow_graph() {
        split_basic_blocks();
        set_blocks_relations();
    }

    void optimization() {
        for (auto & block : blocks) {
            block.optimization();
        }
        global_optimization();
    }

    // 回写qcodes
    void update_qcodes() {
        qcodes.clear();
        for (auto & block : blocks) {
            auto qcodes_buff = block.get_content();
            for (auto & j : qcodes_buff) {
                if(j.type != "nop") qcodes.push_back(j);
            }
        }
    }

private:
    vector<basic_block> blocks;
    map<string, int> entrance2id;
    void split_basic_blocks() {
        vector<Quardcode> qbuffer;
        int block_id = 1;
        for (const auto& pc : qcodes) {
            // 跳转到的入口
            if (pc.type == "set_label" || pc.type == "set_function_flag") {
                vector<int> arr_temp;
                int count_t = 0;
                for (int ii = 0;ii < pc.dst.size();ii++) {
                    if (pc.dst[ii] == '_') count_t++;
                }
                if (count_t != 1 && pc.type == "set_label") {
                    qbuffer.push_back((Quardcode("block_end", "", arr_temp, "", "")));
                    blocks.emplace_back(qbuffer);
                    qbuffer.clear();
                    qbuffer.push_back(pc);
                    vector<int> arr_temp1;
                    qbuffer.push_back((Quardcode("clear_all", "", arr_temp1, "", "")));
                    entrance2id[pc.dst] = block_id;
                    block_id++;
                    continue;
                } else if (count_t == 1 && pc.type == "set_label") {
                    qbuffer.push_back((Quardcode("block_end", "", arr_temp, "", "")));
                    qbuffer.push_back(pc);
                    vector<int> arr_temp1;
                    qbuffer.push_back((Quardcode("clear_all", "", arr_temp1, "", "")));
                    continue;
                }
                else if (pc.type == "set_function_flag") {
                    qbuffer.push_back((Quardcode("block_end", "", arr_temp, "", "")));
                    blocks.emplace_back(qbuffer);
                    qbuffer.clear();
                    qbuffer.push_back(pc);
                    vector<int> arr_temp1;
                    qbuffer.push_back((Quardcode("clear_all", "", arr_temp1, "", "")));
                    entrance2id[pc.dst] = block_id;
                    block_id++;
                    continue;
                }
            }
            else if (pc.type == "call") {
                qbuffer.push_back(pc);
                vector<int> arr_temp;
                qbuffer.push_back((Quardcode("clear_all", "", arr_temp, "", "")));
                continue;
            }
            // 有条件跳转语句
            else if (pc.type == "beq" || pc.type == "bne" || pc.type == "ble" ||
                     pc.type == "blt" || pc.type == "bge" || pc.type == "bgt") {
                vector<int> arr_temp;
                if (!is_case_jump(pc.dst)) {
                    qbuffer.push_back((Quardcode("block_end", "", arr_temp, "", "")));
                    vector<int> arr_temp1;
                    qbuffer.push_back((Quardcode("clear_all", "", arr_temp1, "", "")));
                }

                qbuffer.push_back(pc);
                blocks.emplace_back(qbuffer);
                block_id++;
                qbuffer.clear();
                continue;
            }
            //无条件跳转语句
            else if (pc.type == "jump" ||
                     pc.type == "return" || pc.type == "shut_down") {
                if (pc.type != "shut_down") {
                    vector<int> arr_temp;
                    qbuffer.push_back((Quardcode("block_end", "", arr_temp, "", "")));
                }
                qbuffer.push_back(pc);

                blocks.emplace_back(qbuffer);
                block_id++;
                qbuffer.clear();
                continue;
            }
            qbuffer.push_back(pc);
        }
        // 程序结束也是一个入口语句
        if (!qbuffer.empty()) {
            blocks.emplace_back(qbuffer);
            vector<int> arr_temp1;
            qbuffer.push_back((Quardcode("clear_all", "", arr_temp1, "", "")));
        }
    }

    void set_blocks_relations() {
        int block_id = 1;
        for (const auto& pc : qcodes) {
            // 跳转到的入口
            if (pc.type == "set_label" || pc.type == "set_function_flag") {
                block_id++;
            }
            // 有条件跳转语句
            else if (pc.type == "beq" || pc.type == "bne" || pc.type == "ble" ||
                     pc.type == "blt" || pc.type == "bge" || pc.type == "bgt") {
                blocks[block_id].add_descendent(entrance2id[pc.op2]);
                block_id++;
            }
            // 无条件跳转语句
            else if (pc.type == "jump") {
                blocks[block_id].add_descendent(entrance2id[pc.dst]);
                block_id++;
            }
            // 没有后继
            else if (pc.type == "return" || pc.type == "shut_down") {
                block_id++;
            }
        }
    }

    // 全局优化
    void global_optimization() {
        ;
    }
};

void optimization_for_qcodes() {
    flow_graph fg = flow_graph();
    fg.optimization();
    fg.update_qcodes();
}

#endif //UNTITLED_FLOW_GRAPH_H
