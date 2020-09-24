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

        }
        else if(!strcmp(arg[0], "list_back")){

        }
        else if(!strcmp(arg[0], "list_shuffle")){

        }
        else if(!strcmp(arg[0], "list_swap")){

        }
        else if(!strcmp(arg[0], "list_insert")){

        }
        else if(!strcmp(arg[0], "list_insert_ordered")){

        }
        else if(!strcmp(arg[0], "list_remove")){

        }
        else if(!strcmp(arg[0], "list_sort")){

        }
        else if(!strcmp(arg[0], "list_unique")){

        }
        else if(!strcmp(arg[0], "list_reverse")){

        }
        else if(!strcmp(arg[0], "list_splice")){

        }
        else if(!strcmp(arg[0], "list_empty")){

        }
        else if(!strcmp(arg[0], "list_size")){

        }
        else if(!strcmp(arg[0], "list_max")){

        }
        else if(!strcmp(arg[0], "list_min")){

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
