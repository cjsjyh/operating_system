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

struct list_elem* list_get_element_at(struct list_elem* first, int index);
struct list_elem* list_find_element(struct list_elem* first, int data);
bool custom_list_less_func(const struct list_elem *a, const struct list_elem *b, void *aux);

void bool_to_str(int bit);
int str_to_bool(char* str);

unsigned custom_hash_hash_func (const struct hash_elem *e, void *aux);
bool custom_hash_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux);
void custom_square(struct hash_elem* a, void* aux);
void custom_triple(struct hash_elem* a, void* aux);
void destructor(struct hash_elem *e, void *aux);

#endif