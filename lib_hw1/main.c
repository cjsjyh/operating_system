#include "main.h"

int main(){
    char command[100], arg[10][30];
    int arg_count = 0, type, index;

    my_list lists[20];
    my_hash hashs[20];
    my_bitmap bitmaps[20];
    int list_cnt, hash_cnt, bitmap_cnt;
    list_cnt = hash_cnt = bitmap_cnt = 0;

    struct list_item *temp_list_item;
    struct list_elem *temp_list_elem;
    struct bitmap *temp_bitmap;

    while(1) {
        fgets(command, sizeof command, stdin);
        arg_count = sscanf(command, "%s%s%s%s%s%s%s", arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
        
        // --------------------
        // GENERAL COMMANDS
        // --------------------
        if (!strcmp(arg[0], "quit")){
            return 0;
        }
        else if(!strcmp(arg[0], "test")){
            printf("[Count: %d]\n",list_cnt);
            for(int i =0; i<20; i++)
                printf("%s\n",lists[i].name);
        }
        else if(!strcmp(arg[0], "create")){
            if(!strcmp(arg[1], "list")){
                struct list* new_list = (struct list*)malloc(sizeof(struct list));
                list_init(new_list);
                lists[list_cnt].ptr = new_list;
                strcpy(lists[list_cnt++].name, arg[2]);
            }
            else if(!strcmp(arg[1], "bitmap")){
                temp_bitmap = bitmap_create(atoi(arg[3]));
                bitmaps[bitmap_cnt].ptr = temp_bitmap;
                strcpy(bitmaps[bitmap_cnt++].name, arg[2]);
            }
        }
        else if(!strcmp(arg[0], "dumpdata")){
            if(!find_type_index(lists, list_cnt, hashs, hash_cnt, bitmaps, bitmap_cnt, arg[1], &type, &index))
                printf("Failed to find %s", arg[1]);
            else {
                if(type == LIST){
                    struct list_elem *p;
                    for (p=list_begin(lists[index].ptr); p!=list_end(lists[index].ptr); p=list_next(p))
                        printf("%d ", list_entry(p, struct list_item, elem)->data);
                    if (!list_empty(lists[index].ptr))
                        printf("\n");
                }
                else if(type == BITMAP){
                    for (int j=0; j<(int)bitmap_size(bitmaps[index].ptr); j++) {
                        printf("%d", bitmap_test(bitmaps[index].ptr, j));
                    }
                    printf("\n");
                }
            }
        }
        else if(!strcmp(arg[0], "delete")){
            if(find_type_index(lists, list_cnt, hashs, hash_cnt, bitmaps, bitmap_cnt, arg[1], &type, &index)){
                if(type == 0)
                    free_array(lists, list_cnt--, LIST, index);
                else if(type == 1)
                    free_array(hashs, hash_cnt--, HASH, index);
                else if(type == 2)
                    free_array(bitmaps, bitmap_cnt--, BITMAP, index);
            }
            else
                printf("Failed to find %s", arg[1]);
        }

        // --------------------
        // LIST COMMANDS
        // --------------------
        else if(!strcmp(arg[0], "list_push_front") || !strcmp(arg[0], "list_push_back")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_item = (struct list_item*)malloc(sizeof(struct list_item));
            temp_list_item->data = atoi(arg[2]);
            if(!strcmp(arg[0], "list_push_front"))
                list_push_front(lists[index].ptr, &(temp_list_item->elem));
            else if(!strcmp(arg[0], "list_push_back"))
                list_push_back(lists[index].ptr, &(temp_list_item->elem));
        }
        else if(!strcmp(arg[0], "list_pop_back") || !strcmp(arg[0], "list_pop_front")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            if(!strcmp(arg[0], "list_pop_front"))
                temp_list_elem = list_pop_front(lists[index].ptr);
            else if(!strcmp(arg[0], "list_pop_back"))
                temp_list_elem = list_pop_back(lists[index].ptr);
        }   
        else if(!strcmp(arg[0], "list_front")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_elem = list_front(lists[index].ptr);
            printf("%d\n",((list_item*)temp_list_elem)->data);
        }
        else if(!strcmp(arg[0], "list_back")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_elem = list_back(lists[index].ptr);
            printf("%d\n",((list_item*)temp_list_elem)->data);
        }
        else if(!strcmp(arg[0], "list_shuffle")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            list_shuffle(lists[index].ptr);
        }
        else if(!strcmp(arg[0], "list_swap")){
            int count, temp;
            int first = atoi(arg[2]);
            int second = atoi(arg[3]);
            if(first > second){
                temp = first;
                first = second;
                second = temp;
            }
            index = find_index(lists, list_cnt, LIST, arg[1]);
            struct list_elem *first_ptr = list_get_element_at((lists[index].ptr)->head.next, first);
            struct list_elem *second_ptr = list_get_element_at((lists[index].ptr)->head.next, second);
            list_swap(first_ptr, second_ptr);
        }
        else if(!strcmp(arg[0], "list_insert")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_item = (struct list_item*)malloc(sizeof(struct list_item));
            temp_list_item->data = atoi(arg[3]);

            int size = (int)list_size(lists[index].ptr);
            int insert_at = atoi(arg[2]);
            if (insert_at == 0)
                list_push_front(lists[index].ptr, &(temp_list_item->elem));
            else if (size <= insert_at)
                list_push_back(lists[index].ptr, &(temp_list_item->elem));
            else{
                struct list_elem* before = list_get_element_at((lists[index].ptr)->head.next, insert_at);
                list_insert(before, &(temp_list_item->elem));
            }
        }
        else if(!strcmp(arg[0], "list_insert_ordered")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_item = (struct list_item*)malloc(sizeof(struct list_item));
            temp_list_item->data = atoi(arg[2]);
            list_insert_ordered(lists[index].ptr, &(temp_list_item->elem), custom_list_less_func, NULL);
        }
        else if(!strcmp(arg[0], "list_remove")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_elem = list_get_element_at((lists[index].ptr)->head.next, atoi(arg[2]));
            list_remove(temp_list_elem);
        }
        else if(!strcmp(arg[0], "list_sort")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            list_sort(lists[index].ptr, custom_list_less_func, NULL);
        }
        else if(!strcmp(arg[0], "list_unique")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            struct list *list1 = lists[index].ptr;
            if(arg_count == 3){
                index = find_index(lists, list_cnt, LIST, arg[2]);
                struct list *list2 = lists[index].ptr;
                list_unique(list1, list2, custom_list_less_func, NULL);
            }
            else
                list_unique(list1, NULL, custom_list_less_func, NULL);
        }
        else if(!strcmp(arg[0], "list_reverse")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            list_reverse(lists[index].ptr);
        }
        else if(!strcmp(arg[0], "list_splice")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            struct list_elem *dst = list_get_element_at(lists[index].ptr->head.next, atoi(arg[2]));
            index = find_index(lists, list_cnt, LIST, arg[3]);
            struct list_elem *src_start = list_get_element_at(lists[index].ptr->head.next, atoi(arg[4]));
            struct list_elem *src_end = list_get_element_at(lists[index].ptr->head.next, atoi(arg[5]));
            list_splice(dst, src_start, src_end);
        }
        else if(!strcmp(arg[0], "list_empty")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            int result = (int)list_empty(lists[index].ptr);
            if(result)
                printf("true\n");
            else
                printf("false\n");
        }
        else if(!strcmp(arg[0], "list_size")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            printf("%d\n", (int)list_size(lists[index].ptr));
        }
        else if(!strcmp(arg[0], "list_max")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_elem = list_max(lists[index].ptr, custom_list_less_func, NULL);
            printf("%d\n", ((list_item*)temp_list_elem)->data);
        }
        else if(!strcmp(arg[0], "list_min")){
            index = find_index(lists, list_cnt, LIST, arg[1]);
            temp_list_elem = list_min(lists[index].ptr, custom_list_less_func, NULL);
            printf("%d\n", ((list_item*)temp_list_elem)->data);
        }

        // --------------------
        // BITMAP COMMANDS
        // --------------------
        else if(!strcmp(arg[0], "bitmap_mark")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            bitmap_mark(bitmaps[index].ptr, atoi(arg[2]));
        }
        else if(!strcmp(arg[0], "bitmap_set_all")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int value = str_to_bool(arg[2]);
            bitmap_set_all(bitmaps[index].ptr, value);
        }
        else if(!strcmp(arg[0], "bitmap_set")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int value = str_to_bool(arg[3]);
            bitmap_set(bitmaps[index].ptr, atoi(arg[2]), value);
        }
        else if(!strcmp(arg[0], "bitmap_set_multiple")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int value = str_to_bool(arg[4]);
            bitmap_set_multiple(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]), value);
        }
        else if(!strcmp(arg[0], "bitmap_dump")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            bitmap_dump(bitmaps[index].ptr);
        }
        else if(!strcmp(arg[0], "bitmap_all")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_all(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]));
            bool_to_str(result);
        }
        else if(!strcmp(arg[0], "bitmap_any")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_any(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]));
            bool_to_str(result);
        }
        else if(!strcmp(arg[0], "bitmap_reset")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            bitmap_reset(bitmaps[index].ptr, atoi(arg[2]));
        }
        else if(!strcmp(arg[0], "bitmap_contains")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_contains(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]), str_to_bool(arg[4]));
            bool_to_str(result);
        }
        else if(!strcmp(arg[0], "bitmap_expand")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            bitmap_expand(bitmaps[index].ptr, atoi(arg[2]));
        }
        else if(!strcmp(arg[0], "bitmap_flip")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            bitmap_flip(bitmaps[index].ptr, atoi(arg[2]));
        }
        else if(!strcmp(arg[0], "bitmap_count")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_count(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]), str_to_bool(arg[4]));
            printf("%d\n", result);
        }
        //not tested
        else if(!strcmp(arg[0], "bitmap_none")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_none(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]));
            bool_to_str(result);
        }
        else if(!strcmp(arg[0], "bitmap_scan")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_scan(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]), str_to_bool(arg[4]));
            printf("%u\n", result);
        }
        else if(!strcmp(arg[0], "bitmap_scan_and_flip")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_scan_and_flip(bitmaps[index].ptr, atoi(arg[2]), atoi(arg[3]), str_to_bool(arg[4]));
            printf("%u\n", result);
        }
        else if(!strcmp(arg[0], "bitmap_size")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_size(bitmaps[index].ptr);
            printf("%u\n", result);
        }
        else if(!strcmp(arg[0], "bitmap_test")){
            index = find_index(bitmaps, bitmap_cnt, BITMAP, arg[1]);
            int result = bitmap_test(bitmaps[index].ptr, atoi(arg[2]));
            bool_to_str(result);
        }

        // --------------------
        // HASH COMMANDS
        // --------------------

        else
            printf("Invalid command: %s\n", arg[0]);
    }





}

int find_type_index(my_list* lists, int list_cnt, my_hash* hashs, int hash_cnt, my_bitmap* bitmaps, int bitmap_cnt, char* name, int* type, int* index){
    int result;
    result = find_index(lists, list_cnt, 0, name);
    if(result == -1){
        result = find_index(hashs, hash_cnt, 1, name);
        if(result == -1){
            result = find_index(bitmaps, bitmap_cnt, 2, name);
            if(result == -1)
                return FALSE;
            else{
                *type = BITMAP;
                *index = result;
            }
        }
        else{
            *type = HASH;
            *index = result;
        }
    }
    else{
        *type = LIST;
        *index = result;
    }
    return TRUE;
}

int find_index(void* arr, int len, int type, char* text){
    int i = 0;
    for(i = 0; i<len; i++){
        // Found from my_list
        if(type == LIST && !strcmp(((my_list*)arr)[i].name, text))
            return i;
        else if(type == HASH && !strcmp(((my_hash*)arr)[i].name, text))
            return i;
        else if(type == BITMAP && !strcmp(((my_bitmap*)arr)[i].name, text))
            return i;
    }
    return -1;
}

void free_array(void* arr, int len, int type,int index){
    if(type == HASH){
        
    }
    else
        bitmap_destroy(((my_bitmap*)arr)[index].ptr);
    
    int i = 0;
    for(i = index; i < len - 1; i++){
        if(type == LIST){
            strcpy(((my_list*)arr)[i].name, ((my_list*)arr)[i+1].name);
            ((my_list*)arr)[i].ptr = ((my_list*)arr)[i+1].ptr;
        }
        else if(type == HASH){
            strcpy(((my_hash*)arr)[i].name, ((my_hash*)arr)[i+1].name);
            ((my_hash*)arr)[i].ptr = ((my_hash*)arr)[i+1].ptr;
        }
        else{
            strcpy(((my_bitmap*)arr)[i].name, ((my_bitmap*)arr)[i+1].name);
            ((my_bitmap*)arr)[i].ptr = ((my_bitmap*)arr)[i+1].ptr;
        }
    }
}

// --------------------
// LIST FUNCTIONS
// --------------------

struct list_elem* list_get_element_at(struct list_elem* first, int index){
    struct list_elem* first_ptr = first;
    int count = 0;
    while(count++ < index)
        first_ptr = first_ptr->next;
    return first_ptr;
}

bool custom_list_less_func(const struct list_elem *a, const struct list_elem *b, void *aux) {
	if ((list_entry(a, struct list_item, elem)->data) < (list_entry(b, struct list_item, elem)->data)) return true;
	else return false;
}

struct list_elem* list_find_element(struct list_elem* first, int data){
    struct list_elem* first_ptr = first;
    while(first_ptr != NULL && list_entry(first_ptr, struct list_item, elem)->data != data)
        first_ptr = first_ptr->next;
    return first_ptr;
}

// ----------------------
// BITMAP FUNCTIONS
// ----------------------

void bool_to_str(int bit){
    if(bit == TRUE)
        printf("true\n");
    else
        printf("false\n");
}

int str_to_bool(char* str){
    if(!strcmp(str, "true"))
        return TRUE;
    else
        return FALSE;
}

