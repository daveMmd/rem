#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include "common.h"
#include "SSA.h"

bool cmp(Entry_set E1, Entry_set E2){
    return Elen(E1) > Elen(E2);
}
Entry** MergeGreedy(Entry** E_set, int n){
    Entry** TE_set = (Entry**)malloc(sizeof(Entry*) * (n+5));
    memset(TE_set, 0, sizeof(Entry*)*(n+5));
    for(int i=0; i<n; i++){
        TE_set[i] = (Entry*)malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
        memset(TE_set[i], 0, sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    }
    int m = Eset_len(E_set);
    //sort according the size
    std::sort(E_set, E_set+m, cmp);
    /*for(int i=0; i<m; i++){
        printf("E_set[%d] size:%d\n", i, Elen(E_set[i]));
    }
    printf("MergeGreedy E_set size:%d\n", m);*/
    for(int i=0; i<m; i++){
        uint64_t min_increment = 0xffffffff;
        int choose_index = 0;
        Entry* choose_E = NULL;
        //first find empty TE[i] to merge, if any
        bool flag = false;
        for(int j=0; j<n; j++){
            if(Elen(TE_set[j]) == 0){
                flag = true;
                choose_index = j;
                choose_E = MergeTwo(E_set[i], TE_set[j]);
                break;
            }
        }

        if(!flag){
            for(int j=0; j<n; j++){
                Entry* tmpE = MergeTwo(E_set[i], TE_set[j]);
                uint64_t increment = Elen(tmpE) - Elen(E_set[i]) - Elen(TE_set[j]);
                if(increment < min_increment){
                    min_increment = increment;
                    free_Entry_set(choose_E);
                    choose_E = tmpE;
                    choose_index = j;
                }
                else free_Entry_set(tmpE);
            }
        }
        /*
        printf("##########\n");
        printf("choose_index:%d\n", choose_index);
        printf("target E size: %llu,min_increment:%llu\n", Elen(choose_E), min_increment);
        printf("##########\n");*/
        TE_set[choose_index] = choose_E;
    }
    return TE_set;
}

Entry_set readEntrySet(char* fname, int set_index){
    printf("readEntrySet read from %s\n", fname);
    FILE* f = fopen(fname, "r");
    Entry_set E = (Entry_set)malloc(sizeof(Entry)*MAX_ENTRY_SET_SIZE);
    int cnt=0;
    while(!feof(f)){
        if(cnt>=MAX_ENTRY_SET_SIZE-1){
            printf("entry size exceeds MAX_ENTRY_SET_SIZE!!\n");
            fclose(f);
            return E;
        }
        fscanf(f, "%s", E[cnt].value);
        //printf("E[%d].value:%s\n",cnt,E[cnt].value);
        if(strlen(E[cnt].value) < 1) break;
        char nouse[50];
        fscanf(f, "%s", nouse);

        E[cnt].is_intersec = false;
        E[cnt].set_index = (uint32_t *) malloc(sizeof(int)*MAX_ENTRY_SETS);
        memset(E[cnt].set_index, 0, sizeof(int)*MAX_ENTRY_SETS);
        E[cnt].set_index[0] = ((uint32_t)set_index << 28) | ((uint32_t)(cnt+1));//to verify
        //printf("init set_index:%u\n", E[cnt].set_index[0]);
        //getchar();
        cnt++;
    }
    fclose(f);
    printf("readEntrySet read %d entrys\n", cnt);
    return E;
}

void test_DC(){
    //char directory[30] = "../../DC/entrySet_";
    printf("enter test_DC()\n");
    int set_num = 16;
    Entry_set *E_set = (Entry_set *)malloc(sizeof(Entry_set)*(set_num+1));
    memset(E_set, 0, sizeof(Entry_set)*(set_num+1));
    for(int num=1; num<=set_num; num++){
        char fname[50];
        //sprintf(fname, "../../FW/entrySet_%d", num);
        //sprintf(fname, "../../DC/entrySet_%d", num);
        sprintf(fname, "../../ACL/entrySet_%d", num);
        E_set[num-1] = readEntrySet(fname, num);
    }
    for(int n=set_num; n>=1; n--)
    {

        Entry_set *TE_set = MergeGreedy(E_set, n);
        uint64_t total_size = 0;
        for(int i=0; ;i++){
            if(TE_set[i]==NULL) break;
            total_size += Elen(TE_set[i]);
        }
        printf("**********\n");
        printf("MergeGreedy, target number:%d\n", n);
        printf("total size:%llu\n", total_size);
        printf("**********\n");
        for(int i=0; ;i++){
            if(TE_set[i]==NULL) break;
            free_Entry_set(TE_set[i]);
        }
        free(TE_set);
    }
    for(int i=0; ;i++) {
        if(E_set[i]==NULL) break;
        free_Entry_set(E_set[i]);
    }
    free(E_set);
    printf("exit test_DC()\n");
}

void test_SSA(){
    printf("enter test_SSA()\n");
    int set_num = 16;
    Entry_set *E_set = (Entry_set *)malloc(sizeof(Entry_set)*(set_num+1));
    memset(E_set, 0, sizeof(Entry_set)*(set_num+1));
    for(int num=1; num<=set_num; num++){
        char fname[50];
        //sprintf(fname, "../../FW/entrySet_%d", num);
        //sprintf(fname, "../../IPC/entrySet_%d", num);
        sprintf(fname, "../../DC/entrySet_%d", num);
        //sprintf(fname, "../../ACL/entrySet_%d", num);
        E_set[num-1] = readEntrySet(fname, num);
    }
    /*for(int n=2; n<=4; n++)
    {
        printf("setting target entry-set number:%d\n",n);
        Eset TEset = SSA(E_set, n);
        int total_entries = 0;
        for(int i=0; ;i++){
            if(TEset[i]==NULL) break;
            total_entries += Elen(TEset[i]);
            //printf("test_SSA:locx\n");
            //printf("test_SSA: to free address %x\n", TEset[i]);
            free_Entry_set(TEset[i]);// free
        }
        printf("total entries:%d\n", total_entries);
    }*/
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    SSA_alltest(E_set, 16);
    gettimeofday(&end, NULL);
    printf("Total cost time:%f seconds\n", end.tv_sec - begin.tv_sec + 0.000001 * (end.tv_usec - begin.tv_usec));
    for(int i=0; ;i++) {
        if(E_set[i]==NULL) break;
        free_Entry_set(E_set[i]);
    }
    free(E_set);
    printf("exit test_SSA()\n");
}
int main() {
    std::cout << "Hello, TEM!" << std::endl;
    //test_DC();
    test_SSA();
    return 0;
}