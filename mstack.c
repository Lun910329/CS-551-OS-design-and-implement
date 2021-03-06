#include "stdio.h"
#include "mstack.h"
#include "stdlib.h"
#include "string.h"

/*
 * init stack.
 */
void initstack(mstack **stk){
    *stk = (mstack *)malloc(sizeof(mstack));
    (*stk)->size = 0;
    (*stk)->list = (void *)malloc(sizeof(void*)*MSTACKSIZE);
}

/*
 * push item.
 */
int push (void *item, mstack *stk){
    if(stk->size < MSTACKSIZE){
        stk->list[stk->size] = item;
        return ++stk->size;
    } else {
        return -1;
    }
}

/*
 * pop item.
 */
int pop(void **rv, mstack *stk){
    if(stk->size >0){
        *rv = stk->list[--stk->size];
        return stk->size;
    } else {
        return -1;
    }
}

/*
 * free stack.
 */
void freestack(mstack *stk){
    free(stk->list);
    free(stk);
}

