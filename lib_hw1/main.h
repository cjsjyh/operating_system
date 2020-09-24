#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "limits.h"
#include "round.h"
#include "debug.h"

typedef struct my_list {
    struct list *ptr;
    char name[30];
} my_list;

typedef struct my_hash {
    struct hash *ptr;
    char name[30];
} my_hash;

typedef struct my_bitmap {
    struct bitmap *ptr;
    char name[30];
} my_bitmap;


int is_in_array(void* , int , int , char* );
void free_array(void* arr, int len, int type,int index);
