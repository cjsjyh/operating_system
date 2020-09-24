#include "main.h"

int main(){
    char command[100], arg[10][20];
    int arg_count = 0;

    my_list lists[1000];
    my_hash hashs[1000];
    my_bitmap bitmaps[1000];
    int list_count, hash_count, bitmap_count;
    list_count = hash_count = bitmap_count = 0;

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
            

        }
        else if(!strcmp(arg[0], "delete")){
            int result;
            result = is_in_array(lists, list_count, 0, arg[1]);
            if(result == -1){
                result = is_in_array(hashs, hash_count, 1, arg[1]);
                if(result == -1){
                    result = is_in_array(bitmaps, bitmap_count, 2, arg[1]);
                    if(result == -1)
                        printf("Name not found\n");
                    else{
                        //free bitmap
                        free_array(bitmaps, bitmap_count--, 2, result);
                    }
                }
                else{
                    //free hash
                    free_array(hashs, hash_count--, 1, result);
                }
            }
            else{
                //free lists
                free_array(lists, list_count--, 0, result);
            }
        }


        // --------------------
        // LIST COMMANDS
        // --------------------
        else if(!strcmp(arg[0], "list_push_front")){
             
        }     
        else if(!strcmp(arg[0], "list_push_back")){
            
        }   
        else if(!strcmp(arg[0], "list_pop_back")){

        }   
        else if(!strcmp(arg[0], "list_pop_front")){

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
    }











    //printf("%s %s %s %s %s", arg[0], arg[1], arg[2], arg[3], arg[4]);
}

int is_in_array(void* arr, int len, int type, char* text){
    int i = 0;
    for(i = 0; i<len; i++){
        // Found from my_list
        if(type == 0 && !strcmp(((my_list*)arr)[i].name, text))
            return i;
        else if(type == 1 && !strcmp(((my_hash*)arr)[i].name, text))
            return i;
        else if(type == 2 && !strcmp(((my_bitmap*)arr)[i].name, text))
            return i;
    }
    return -1;
}

void free_array(void* arr, int len, int type,int index){
    int i = 0;
    for(i = index; i < len - 1; i++){
        if(type == 0){
            ((my_list*)arr)[index].ptr = ((my_list*)arr)[index+1].ptr;
            strcpy(((my_list*)arr)[index].name, ((my_list*)arr)[index+1].name);
        }
        else if(type == 1){
            ((my_hash*)arr)[index].ptr = ((my_hash*)arr)[index+1].ptr;
            strcpy(((my_hash*)arr)[index].name, ((my_hash*)arr)[index+1].name);
        }
        else{
            ((my_bitmap*)arr)[index].ptr = ((my_bitmap*)arr)[index+1].ptr;
            strcpy(((my_bitmap*)arr)[index].name, ((my_bitmap*)arr)[index+1].name);
        }
    }
}
