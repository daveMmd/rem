//
// Created by 钟金诚 on 2020/2/17.
//

#ifndef TEM_CLION_COMMON_H
#define TEM_CLION_COMMON_H

#include <cstdint>

#define MAX_ENTRY_SET_SIZE 512*1024 //标示512K
#define ENTRY_WIDTH 104
#define RULE_ID_LENGTH 20
#define MAX_ENTRY_SETS 17

typedef struct _entry{
    char value[ENTRY_WIDTH+5];
    bool is_intersec; //是否为产生的交项
    //4+28, i1-i2
    uint32_t* set_index; //位于的项集坐标，交项存在多个坐标. 1开始计数，0标示结尾
    // NULL标示entry为空
}Entry;

typedef Entry* Entry_set;
typedef Entry_set* Eset;

uint64_t Elen(Entry_set E);

uint64_t Eset_len(Entry_set* E);

uint64_t Mlen(char** match_lis);

void free_Entry(Entry e);

void free_Entry_set(Entry_set E);

Entry intersec(Entry e1, Entry e2);

bool coveredBy(Entry e, Entry_set E);

//深拷贝match_list
void deepcopy(Entry &e);

Entry_set MergeTwo(Entry_set E1, Entry_set E2);


Entry_set MergeArbitray(Eset OEset);

#endif //TEM_CLION_COMMON_H
