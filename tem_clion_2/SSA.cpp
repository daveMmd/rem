//
// Created by 钟金诚 on 2020/2/17.
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <assert.h>
#include <sys/time.h>
#include "common.h"

typedef struct _clause{
    bool removed;
    float weight;//0 indicates the end
    uint32_t literals[MAX_ENTRY_SETS];
}Clause;

//calc number of intersections for a entry-set
int get_amount(Entry_set E){
    int num = 0;
    for(uint64_t i=0; ; i++) {
        if(E[i].set_index == NULL) break;
        if(E[i].is_intersec) num++;
    }
    //printf("get_amount: num-%d\n",num);
    return num;
}

uint32_t Completment(uint32_t l){
    return ~l;
}

float totalWeight(uint32_t lit, Clause* clause){
    float weight = 0;
    for(int i=0; ;i++){
        if(clause[i].removed) continue;
        if(clause[i].weight == 0) break;
        for(int j=0; ;j++){
            if(clause[i].literals[j]==0) break;
            if(clause[i].literals[j] == lit){
                //printf("totalWeight:succ in\n");
                weight += clause[i].weight;
                break;
            }
        }
    }
    return weight;
}

void removeClauses(uint32_t lit, Clause* clause){
    for(int i=0; ;i++){
        if(clause[i].removed) continue;
        if(clause[i].weight == 0) break;
        for(int j=0; ;j++){
            if(clause[i].literals[j]==0) break;
            if(clause[i].literals[j] == lit){
                clause[i].removed = true;
                break;
            }
        }
    }
}

void doubleClauses(uint32_t lit, Clause* clause){
    for(int i=0; ;i++){
        if(clause[i].removed) continue;
        if(clause[i].weight == 0) break;
        for(int j=0; ;j++){
            if(clause[i].literals[j]==0) break;
            if(clause[i].literals[j] == lit){
                clause[i].weight = 2*clause[i].weight;
                break;
            }
        }
    }
}

Entry_set merge_one_group(Entry_set E){
    //split to several sets according to set-index
    //printf("merge_one_group:enter\n");
    Entry_set Es[18];
    for(int i=0; i<17; i++){
        Es[i] = (Entry_set) malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
        memset(Es[i], 0, sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    }
    Es[17] = NULL;
    int N[17];
    memset(N, 0, sizeof(int)*17);
    for(int i=0; ;i++){
        if(E[i].set_index == NULL) break;
        uint32_t ss_index = E[i].set_index[0] >> 28;
        Es[ss_index][N[ss_index]] = E[i];
        deepcopy(Es[ss_index][N[ss_index]++]);
    }

    free_Entry_set(E);

    Entry_set TE = MergeArbitray(Es);
    //printf("merge_one_group: len(TE)-%llu\n", Elen(TE));

    for(int i=0; i<17; i++)
    {
        //printf("merge_one_group: freed address -- %x\n", Es[i]);
        free_Entry_set(Es[i]);
    }
    //printf("merge_one_group:out\n");
    return TE;
}

Eset merge_groups(Eset TWOset){
    //printf("merge_groups:enter\n");
    Eset TEset = (Eset) malloc(sizeof(Entry_set)*2);
    TEset[0] = merge_one_group(TWOset[0]);
    TEset[1] = merge_one_group(TWOset[1]);
    //printf("merge_groups:exit\n");
    return TEset;
}

//split a merged entry-set into two entry-sets using SSA algorithm
Eset SSA_split(Entry_set E){
    //gen clauses
    //printf("SSA_split: loc0\n");
    Clause* clauses = (Clause*) malloc(sizeof(Clause)*MAX_ENTRY_SET_SIZE*2);
    memset(clauses, 0, sizeof(Clause)*MAX_ENTRY_SET_SIZE*2);
    int M = 0;
    for(uint64_t i=0; ; i++) {
        if(E[i].set_index == NULL) break;
        if(E[i].is_intersec){
            Entry e = E[i];
            //Clause c = clauses[M];
            //Clause c2 = clauses[M+1];
            int ln = 0;
            for(int j=0; ;j++){
                ln = j;
                if(e.set_index[j]==0) break;
                clauses[M].literals[j] = e.set_index[j];
                clauses[M+1].literals[j] = Completment(e.set_index[j]);
            }
            //c.literals[ln] = 0;
            //c2.literals[ln] = 0;
            clauses[M].weight = pow(2, -ln);
            clauses[M+1].weight = pow(2, -ln);
            //printf("SSA_split:c.weight-%f\n",c.weight);
            //printf("SSA_split:c2.weight-%f\n",c2.weight);

            M+=2;
        }
    }
    printf("SSA_split: number of clauses-%d\n", M);

    //get all literals
    Entry_set allLiterals = (Entry_set) malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    memset(allLiterals, 0, sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    int lcnt = 0;
    for(uint64_t i=0; ; i++) {
        if (E[i].set_index == NULL) break;
        if (E[i].is_intersec) continue;
        Entry e = E[i];
        assert(e.set_index[1]==0);
        assert(e.set_index[0]!=0);
        allLiterals[lcnt] = e;
        deepcopy(allLiterals[lcnt++]);
    }


    //printf("SSA_split: loc4\n");
    Eset TWOset = (Eset) malloc(sizeof(Entry_set)*2);
    //memset(TWOset, 0, sizeof(Entry_set)*2);
    TWOset[0] = (Entry_set) malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    TWOset[1] = (Entry_set) malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    memset(TWOset[0], 0, sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    memset(TWOset[1], 0, sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    int n1 =0, n2=0;
    //assign value -- split into two subsets
    for(int i=0; i<lcnt; i++){
        //printf("SSA_split: loc5\n");
        uint32_t lit = allLiterals[i].set_index[0];
        Entry e = allLiterals[i];
        deepcopy(e);
        //printf("SSA_split: loc6\n");
        float w1 = totalWeight(lit, clauses);
        float w2 = totalWeight(~lit, clauses);
        //printf("SSA_split: lit - %u\n", lit);
        //printf("SSA_split: w1-%f\n", w1);
        //printf("SSA_split: w2-%f\n", w2);
        if(w1 > w2){
            //printf("SSA_split: loca\n");
            TWOset[0][n1++] = e;
            removeClauses(lit, clauses);
            doubleClauses(~lit, clauses);
        }
        else{
            //printf("SSA_split: loc7\n");
            TWOset[1][n2++] = e;
            //printf("SSA_split: locb\n");
            removeClauses(~lit, clauses);
            //printf("SSA_split: locc\n");
            doubleClauses(lit, clauses);
            //printf("SSA_split: locd\n");
        }
        //printf("SSA_split: loc8\n");
    }

    free(clauses);
    //printf("SSA_split: loce\n");
    free_Entry_set(allLiterals);
    //printf("SSA_split: loc9\n");

    return merge_groups(TWOset);
}

//SSA
//input: Eset: a set of entry-sets; tn:the number of target-entry-sets
//output: tn target entry-sets: TE1, TE2...
Eset SSA(Eset OEset, int tn){
    if(Eset_len(OEset) <= tn){
        return OEset;
    }
    if(tn >= 16)
    {
        printf("tn too large!\n");
        exit(-1);
    }
    Eset TEset = (Eset) malloc(sizeof(Entry_set) * 17);
    memset(TEset, 0, sizeof(Entry_set)*17);
    TEset[0] = MergeArbitray(OEset);
    printf("SSA:MergeArbitray in one set number:%llu\n", Elen(TEset[0]));
    for(int i=0; i<tn-1; i++)//split tn-1 times
    {
        printf("SSA:split %dth time\n", i+1);
        //choose TE with most intersections
        int most_index = 0;
        int max_amount = -1;
        for(int j=0; j<i+1; j++){
            int amount = get_amount(TEset[j]);
            if( amount > max_amount){
                most_index = j;
                max_amount = amount;
            }
        }
        Eset TWOset = SSA_split(TEset[most_index]);
        free_Entry_set(TEset[most_index]);
        TEset[most_index] = TWOset[0];
        TEset[i+1] = TWOset[1];
        //printf("SSA:loc9\n");
    }

    return TEset;
}

void SSA_alltest(Eset OEset, int max_tn){
    if(max_tn > 16)
    {
        printf("tn too large!\n");
        exit(-1);
    }
    Eset TEset = (Eset) malloc(sizeof(Entry_set) * 17);
    memset(TEset, 0, sizeof(Entry_set)*17);

    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    TEset[0] = MergeArbitray(OEset);
    gettimeofday(&end, NULL);
    printf("SSA:MergeArbitray cost time:%f seconds\n", end.tv_sec - begin.tv_sec + 0.000001 * (end.tv_usec - begin.tv_usec));
    printf("SSA:MergeArbitray in one set number:%llu\n", Elen(TEset[0]));
    for(int i=0; i<max_tn-1; i++)//split tn-1 times
    {
        printf("now tn number:%d\n", i+1);
        int total_entries = 0;
        for(int j=0; j<=i; j++){
            total_entries += Elen(TEset[j]);
        }
        printf("total entries:%d\n", total_entries);
        printf("SSA:split %dth time\n", i+1);
        //choose TE with most intersections
        int most_index = 0;
        int max_amount = -1;
        for(int j=0; j<i+1; j++){
            int amount = get_amount(TEset[j]);
            if( amount > max_amount){
                most_index = j;
                max_amount = amount;
            }
        }
        gettimeofday(&begin, NULL);
        Eset TWOset = SSA_split(TEset[most_index]);
        gettimeofday(&end, NULL);
        printf("SSA:split cost time:%f seconds\n", end.tv_sec - begin.tv_sec + 0.000001 * (end.tv_usec - begin.tv_usec));
        free_Entry_set(TEset[most_index]);
        TEset[most_index] = TWOset[0];
        TEset[i+1] = TWOset[1];
        //printf("SSA:loc9\n");
    }
    for(int i=0; i<max_tn; i++) free_Entry_set(TEset[i]);
}