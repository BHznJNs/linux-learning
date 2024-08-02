#include <stdlib.h>
#include "dict.h"

static void reset_dict(Dict* ht) {
    ht->table     = NULL;
    ht->size      = 0;
    ht->size_mask = 0;
    ht->used      = 0;
}

Dict* create_dict(void) {
    Dict* ht = malloc(sizeof(Dict));
    reset_dict(ht);
    return ht;
}

int dict_add(Dict* dict, void* key, void* val) {
    // 
}
