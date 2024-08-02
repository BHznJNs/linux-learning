#ifndef __DICT_H__
#define __DICT_H__

#define DICT_INITIAL_SIZE 4

typedef struct DictEntry {
    void* key;
    void* val;
    struct DictEntry* next;
} DictEntry;

typedef struct Dict {
    DictEntry** table;
    unsigned long size;
    unsigned long size_mask;
    unsigned long used;
} Dict;

typedef struct dictIterator {
    Dict *ht;
    int index;
    DictEntry* entry;
    DictEntry* nextEntry;
} dictIterator;

// --- --- --- --- --- ---

Dict* create_dict(void);

int dict_add(Dict* dict, void* key, void* val);

#endif
