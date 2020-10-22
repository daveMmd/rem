//
// Created by 钟金诚 on 2020/2/17.
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "common.h"
uint64_t Elen(Entry_set E){
    for(uint64_t i=0; ; i++)
        if(E[i].set_index == NULL) return i;
}

uint64_t Eset_len(Entry_set* E){
    for(uint64_t i=0; ; i++)
        if(E[i] == NULL) return i;
}


void free_Entry(Entry e){
    if(e.set_index == NULL) return;
    free(e.set_index);
}

void free_Entry_set(Entry_set E){
    if(E==NULL) return;
    uint64_t n = Elen(E);
    for(int i=0; i<n; i++) free_Entry(E[i]);
    free(E);
}

Entry intersec(Entry e1, Entry e2){
    //printf("enter intersec()\n");
    Entry e;
    e.set_index = NULL;
    for(int i=0; i<ENTRY_WIDTH; i++){
        if(e1.value[i] == e2.value[i]) e.value[i] = e1.value[i];
        else if(e1.value[i] == '*') e.value[i] = e2.value[i];
        else if(e2.value[i] == '*') e.value[i] = e1.value[i];
        else return e;
    }
    e.is_intersec = true;
    int cnt_index = 0;
    e.set_index = (uint32_t*) malloc(sizeof(int)*MAX_ENTRY_SETS);
    memset(e.set_index, 0, sizeof(int)*MAX_ENTRY_SETS);
    for(int i=0; ;i++){
        if(e1.set_index[i] == 0) break;
        e.set_index[cnt_index++] = e1.set_index[i];
    }
    for(int i=0; ;i++){
        if(e2.set_index[i] == 0) break;
        e.set_index[cnt_index++] = e2.set_index[i];
    }
    return e;
}

bool coveredBy(Entry e, Entry_set E){
    /*in inner-loop, time-consuming*/
    return false;
}

//深拷贝match_list
void deepcopy(Entry &e){
    //printf("enter deepcopy()\n");

    uint32_t* old_set_index = e.set_index;
    e.set_index = (uint32_t *) malloc(sizeof(int)*MAX_ENTRY_SETS);
    memcpy(e.set_index, old_set_index, sizeof(int)*MAX_ENTRY_SETS);
    /*memset(e.set_index, 0, sizeof(int)*MAX_ENTRY_SETS);
    for(int i=0; ; i++){
        if(old_set_index[i] == 0) break;
        e.set_index[i] = old_set_index[i];
    }*/
    //printf("exit deepcopy()\n");
}

Entry_set MergeTwo(Entry_set E1, Entry_set E2){
    Entry_set E = (Entry_set)malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    memset(E, 0, sizeof(Entry)*MAX_ENTRY_SET_SIZE);

    int cnt=0;//E的末尾坐标
    int n1 = Elen(E1);
    int n2 = Elen(E2);
    //printf("MergeTwo E1 size:%d\n",n1);
    //printf("MergeTwo E2 size:%d\n",n2);
    for(int i=0; i<n1; i++){
        for(int j=0; j<n2; j++){
            //printf("loc0\n");
            Entry e = intersec(E1[i], E2[j]);
            if(e.set_index == NULL) continue;
            else if(!coveredBy(e, E)){
                //printf("loc3\n");
                if(cnt>=MAX_ENTRY_SET_SIZE-1){
                    printf("entry size exceeds MAX_ENTRY_SET_SIZE!!\n");
                    return E;
                }
                //printf("loc4\n");
                E[cnt++] = e; //直接赋值，为浅拷贝, 切忌free旧结构体指针
                //deepcopy(E[cnt], e);
                //cnt++;
            }
            //printf("loc5\n");
        }
    }
    for(int i=0; i<n1; i++)
        if(!coveredBy(E1[i], E)){
            //printf("loc6\n");
            if(cnt>=MAX_ENTRY_SET_SIZE-1){
                printf("entry size exceeds MAX_ENTRY_SET_SIZE!!\n");
                return E;
            }
            E[cnt] = E1[i];
            //printf("loc7\n");
            deepcopy(E[cnt]);
            cnt++;
            //printf("loc8\n");

        }
    for(int i=0; i<n2; i++)
        if(!coveredBy(E2[i], E)){
            //printf("loc9\n");
            if(cnt>=MAX_ENTRY_SET_SIZE-1){
                printf("entry size exceeds MAX_ENTRY_SET_SIZE!!\n");
                return E;
            }
            E[cnt] = E2[i];
            deepcopy(E[cnt]);
            cnt++;
            //printf("locx\n");
        }
    //printf("MergeTwo E size:%d\n",cnt);
    return E;
}

Entry_set MergeArbitray(Eset OEset){
    int m = Eset_len(OEset);
    //assert(m >= 2);
    //printf("MergeArbitray:m=%d\n",m);
    Entry_set TE = OEset[0];
    //printf("MergeArbitray: init TE address -- %x\n", TE);
    for(int i=1; i<m; i++)
    {
        Entry_set tmpE = MergeTwo(TE, OEset[i]);
        if(i!=1) free_Entry_set(TE);
        TE = tmpE;
    }
    //printf("MergeArbitray: return TE address -- %x\n", TE);
    return TE;
}