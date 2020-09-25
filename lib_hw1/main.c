#include "main.h"

int main(){
    char command[100], arg[10][20];
    int arg_count = 0, type, index;

    my_list lists[1000];
    my_hash hashs[1000];
    my_bitmap bitmaps[1000];
    int list_count, hash_count, bitmap_count;
    list_count = hash_count = bitmap_count = 0;

    struct list_item *temp_list_item;
    struct list_elem *temp_list_elem;

    while(1) {
        fgets(command, sizeof command, stdin);
        arg_count = sscanf(command, "%s%s%s%s%s%s%s", arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
        
        // --------------------
        // GENERAL COMMANDS
        // --------------------
        if (!strcmp(arg[0], "quit")){
            return 0;
        }
        else if(!strcmp(arg[0], "create")){
            if(!strcmp(arg[1], "list")){
                struct list* new_list = (struct list*)malloc(sizeof(struct list));
                list_init(new_list);
                lists[list_count].ptr = new_list;
                strcpy(lists[list_count++].name, arg[2]);
            }
        }
        else if(!strcmp(arg[0], "dumpdata")){
            if(find_type_index(lists, list_count, hashs, hash_count, bitmaps, bitmap_count, arg[1], &type, &index)){
                if(type == LIST){
                    struct list_elem *p;
                    for (p=list_begin(lists[index].ptr); p!=list_end(lists[index].ptr); p=list_next(p))
                        printf("%d ", list_entry(p, struct list_item, elem)->data);
                    if (!list_empty(lists[index].ptr))
                        printf("\n");
                }
            }
            else
                printf("Failed to find %s", arg[1]);
        }
        else if(!strcmp(arg[0], "delete")){
            if(find_type_index(lists, list_count, hashs, hash_count, bitmaps, bitmap_count, arg[1], &type, &index)){
                if(type == 0)
                    free_array(lists, list_count--, LIST, index);
                else if(type == 1)
                    free_array(hashs, hash_count--, HASH, index);
                else if(type == 2)
                    free_array(bitmaps, bitmap_count--, BITMAP, index);
            }
            else
                printf("Failed to find %s", arg[1]);
        }

        // --------------------
        // LIST COMMANDS
        // --------------------
        else if(!strcmp(arg[0], "list_push_front") || !strcmp(arg[0], "list_push_back")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_item = (struct list_item*)malloc(sizeof(struct list_item));
            temp_list_item->data = atoi(arg[2]);
            if(!strcmp(arg[0], "list_push_front"))
                list_push_front(lists[index].ptr, &(temp_list_item->elem));
            else if(!strcmp(arg[0], "list_push_back"))
                list_push_back(lists[index].ptr, &(temp_list_item->elem));
        }
        else if(!strcmp(arg[0], "list_pop_back") || !strcmp(arg[0], "list_pop_front")){
            index = find_index(lists, list_count, LIST, arg[1]);
            if(!strcmp(arg[0], "list_pop_front"))
                temp_list_elem = list_pop_front(lists[index].ptr);
            else if(!strcmp(arg[0], "list_pop_back"))
                temp_list_elem = list_pop_back(lists[index].ptr);
        }   
        else if(!strcmp(arg[0], "list_front")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_elem = list_front(lists[index].ptr);
            printf("%d\n",((list_item*)temp_list_elem)->data);
        }
        else if(!strcmp(arg[0], "list_back")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_elem = list_back(lists[index].ptr);
            printf("%d\n",((list_item*)temp_list_elem)->data);
        }
        else if(!strcmp(arg[0], "list_shuffle")){
            index = find_index(lists, list_count, LIST, arg[1]);
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
            index = find_index(lists, list_count, LIST, arg[1]);
            struct list_elem *first_ptr = list_get_element_at((lists[index].ptr)->head.next, first);
            struct list_elem *second_ptr = list_get_element_at((lists[index].ptr)->head.next, second);
            list_swap(first_ptr, second_ptr);
        }
        else if(!strcmp(arg[0], "list_insert")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_item = (struct list_item*)malloc(sizeof(struct list_item));
            temp_list_item->data = atoi(arg[3]);

            int size = (int)list_size(lists[index].ptr);
            int insert_at = atoi(arg[2]);
            if (insert_at == 0)
                list_push_front(lists[index].ptr, &(temp_list_item->elem));
            else if (size == insert_at)
                list_push_back(lists[index].ptr, &(temp_list_item->elem));
            else{
                struct list_elem* before = list_get_element_at((lists[index].ptr)->head.next, insert_at);
                list_insert(before, &(temp_list_item->elem));
            }
        }
        else if(!strcmp(arg[0], "list_insert_ordered")){

        }
        else if(!strcmp(arg[0], "list_remove")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_elem = list_find_element((lists[index].ptr)->head.next, atoi(arg[2]));
            list_remove(temp_list_elem);
        }
        else if(!strcmp(arg[0], "list_sort")){
            index = find_index(lists, list_count, LIST, arg[1]);
            list_sort(lists[index].ptr, custom_list_less_func, NULL);
        }
        else if(!strcmp(arg[0], "list_unique")){
            index = find_index(lists, list_count, LIST, arg[1]);
            struct list *list1 = lists[index].ptr;
            index = find_index(lists, list_count, LIST, arg[2]);
            struct list *list2 = lists[index].ptr;
            list_unique(list1, list2, custom_list_less_func, NULL);
        }
        else if(!strcmp(arg[0], "list_reverse")){
            index = find_index(lists, list_count, LIST, arg[1]);
            list_reverse(lists[index].ptr);
        }
        else if(!strcmp(arg[0], "list_splice")){

        }
        else if(!strcmp(arg[0], "list_empty")){
            index = find_index(lists, list_count, LIST, arg[1]);
            int result = (int)list_empty(lists[index].ptr);
            if(result)
                printf("true\n");
            else
                printf("false\n");
        }
        else if(!strcmp(arg[0], "list_size")){
            index = find_index(lists, list_count, LIST, arg[1]);
            printf("%d\n", (int)list_size(lists[index].ptr));
        }
        else if(!strcmp(arg[0], "list_max")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_elem = list_max(lists[index].ptr, custom_list_less_func, NULL);
            printf("%d\n", ((list_item*)temp_list_elem)->data);
        }
        else if(!strcmp(arg[0], "list_min")){
            index = find_index(lists, list_count, LIST, arg[1]);
            temp_list_elem = list_min(lists[index].ptr, custom_list_less_func, NULL);
            printf("%d\n", ((list_item*)temp_list_elem)->data);
        }
        else
            printf("Invalid command\n");
    }





}

int find_type_index(my_list* lists, int list_count, my_hash* hashs, int hash_count, my_bitmap* bitmaps, int bitmap_count, char* name, int* type, int* index){
    int result;
    result = find_index(lists, list_count, 0, name);
    if(result == -1){
        result = find_index(hashs, hash_count, 1, name);
        if(result == -1){
            result = find_index(bitmaps, bitmap_count, 2, name);
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
    int i = 0;
    for(i = index; i < len - 1; i++){
        if(type == LIST){
            ((my_list*)arr)[index].ptr = ((my_list*)arr)[index+1].ptr;
            strcpy(((my_list*)arr)[index].name, ((my_list*)arr)[index+1].name);
        }
        else if(type == HASH){
            ((my_hash*)arr)[index].ptr = ((my_hash*)arr)[index+1].ptr;
            strcpy(((my_hash*)arr)[index].name, ((my_hash*)arr)[index+1].name);
        }
        else{
            ((my_bitmap*)arr)[index].ptr = ((my_bitmap*)arr)[index+1].ptr;
            strcpy(((my_bitmap*)arr)[index].name, ((my_bitmap*)arr)[index+1].name);
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