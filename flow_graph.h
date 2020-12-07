//
// Created by 23207 on 2020/11/21.
//

#ifndef UNTITLED_FLOW_GRAPH_H
#define UNTITLED_FLOW_GRAPH_H
#include "mips.h"
#include "basic_block.h"

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
                blocks.emplace_back(qbuffer);
                entrance2id[pc.dst] = block_id;
                block_id++;
                qbuffer.clear();
            }
            // 有条件跳转语句
            else if (pc.type == "beq" || pc.type == "bne" || pc.type == "ble" ||
                     pc.type == "blt" || pc.type == "bge" || pc.type == "bgt") {
                qbuffer.push_back(pc);
                blocks.emplace_back(qbuffer);
                block_id++;
                qbuffer.clear();
                continue;
            }
            //无条件跳转语句
            else if (pc.type == "jump" ||
                     pc.type == "return" || pc.type == "shut_down") {
                qbuffer.push_back(pc);
                blocks.emplace_back(qbuffer);
                block_id++;
                qbuffer.clear();
                continue;
            }
            qbuffer.push_back(pc);
        }
        // 程序结束也是一个入口语句
        if (!qbuffer.empty()) blocks.emplace_back(qbuffer);
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
