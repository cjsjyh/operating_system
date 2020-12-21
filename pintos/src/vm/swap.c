#include "vm/swap.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "devices/block.h"
#include "threads/synch.h"
#include <bitmap.h>

struct semaphore swap_lock;
struct bitmap *swap_map;
struct block *swap_block;

void swap_init(void){
    swap_block = block_get_role(BLOCK_SWAP);
    if (swap_block == NULL)
        return;
    
    swap_map = bitmap_create(block_size(swap_block) / SECTORS_PER_PAGE);
    if (swap_map == NULL)
        return;
    bitmap_set_all(swap_map, SWAP_FREE);
    
    sema_init(&swap_lock, 1);
}

// swap slot에 저장된 데이터를 논리 주소 kaddr로 복사 (즉, swap영역에서 빠져서 메모리로 들어가는 것)
void swap_in(size_t used_index, void* kaddr){
    sema_down(&swap_lock);
    
    // swap 영역에 있는 페이지를 불러와야되는데 없음
    if(bitmap_test(swap_map, used_index) == SWAP_FREE){
        sema_up(&swap_lock);
        return;
    }

    // kaddr로 복사
    for(int i=0; i<SECTORS_PER_PAGE; i++)
        block_read(swap_block, used_index*SECTORS_PER_PAGE + i, (uint8_t*)kaddr + i*BLOCK_SECTOR_SIZE);
    // swap bitmap을 free로 표시
    bitmap_flip(swap_map, used_index);
    sema_up(&swap_lock);
}


// kaddr에 있는 페이지를 스왑에 기록
size_t swap_out(void* kaddr){
    sema_down(&swap_lock);

    //SWAP_FREE인 swap slot을 찾아서 반전시켜준다(SWAP_USED)
    int free_index = bitmap_scan_and_flip(swap_map, 0, 1, SWAP_FREE);
    if(free_index == BITMAP_ERROR){
        sema_up(&swap_lock);
        return BITMAP_ERROR;
    }
    for(int i=0; i<SECTORS_PER_PAGE; i++)
        block_write(swap_block, free_index*SECTORS_PER_PAGE+i, (uint8_t*)kaddr + i*BLOCK_SECTOR_SIZE);

    sema_up(&swap_lock);
    return free_index;
}

