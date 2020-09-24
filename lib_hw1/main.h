#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "limits.h"
#include "round.h"
#include "debug.h"

#define LIST 0
#define HASH 1
#define BITMAP 2

#define FALSE 0
#define TRUE 1

typedef struct my_list {
    struct list *ptr;
    char name[30];
} my_list;

typedef struct list_item {
    struct list_elem elem;
    int data;
} list_item;

typedef struct my_hash {
    struct hash *ptr;
    char name[30];
} my_hash;

typedef struct hash_item {
	struct hash_elem elem;
	int data;
} hash_item;

typedef struct my_bitmap {
    struct bitmap *ptr;
    char name[30];
} my_bitmap;


int find_type_index(my_list* lists, int list_count, my_hash* hashs, int hash_count, my_bitmap* bitmaps, int bitmap_count, char* name, int* type, int* index);
int find_index(void* , int , int , char* );
void free_array(void* arr, int len, int type,int index);


#endif