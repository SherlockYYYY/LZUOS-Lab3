#include <assert.h>
#include <kdebug.h>
#include <mm.h>

#define MAX_INDEX 11   ///1 ,2,4....1024 共11个块链表


struct buddy {   
    unsigned int index;   ///指数，2^index
    struct buddy *next;
};

struct buddy *FreePageBlock_in_List[MAX_INDEX];  ///指向每个块链表中第一个空闲页框块地址(指向头节点)

void FreePageBlock_in_List_Info () {  ///打印出链表中每个空闲页框的起始地址
    kprintf("Free Page Blocks in List are below :\n");

    for (int i = 0; i < MAX_INDEX; ++i)
     {
        struct buddy *temp = FreePageBlock_in_List[i];
        kprintf("The list of index %d: ", i);
        while (temp!=NULL) 
        {
            kprintf("%p -> ", temp);
            temp = temp->next;
        }
        kprintf("NULL\n");
    }
}

void *memory_distribution(unsigned int index) {
    kputs("\n\n------------------------------------------\n");
    kputs(" Buddy System distribute now！\n");

    assert(index < MAX_INDEX, "Index exceeds the max index,failed!");

    for (unsigned int current_index = index; current_index < MAX_INDEX; ++current_index) 
    {
        if (FreePageBlock_in_List [current_index])
         {
            struct buddy *pages_distributed = FreePageBlock_in_List [current_index];

            FreePageBlock_in_List [current_index] = pages_distributed->next;  ///被分配之后第一个页框块已经不是空闲，去掉它

            while (current_index > index) 
            {
                --current_index;
                long long unsigned page_block_size = 1 << (current_index + 12);  ///计算相对于pages_distributed偏移量
                struct buddy *split_block_start_addr = (struct buddy *)((char *)pages_distributed + page_block_size);  ///计算放入块之后的结束地址，也就是当前空闲块的开始地址
                
                split_block_start_addr->index = current_index;
                split_block_start_addr->next = FreePageBlock_in_List [current_index]; ///把分裂出来的内存块插入FreePageBlock_in_List头部
                FreePageBlock_in_List [current_index] = split_block_start_addr;  ///把现在分裂并回收后完整链表赋值给FreePageBlock_in_List
            }

            kprintf("the address %p is distributed\n", pages_distributed);
            FreePageBlock_in_List_Info ();
            return pages_distributed;
        }
    }

    return NULL;
}

void list_init(unsigned int index, unsigned int loop) {
    // 遍历每个需要初始化的页框块
    for (unsigned int i = 0; i < loop; ++i) {
        unsigned long long current_index_page_size = 1 << (index + 12); // 计算页框块的大小 12为 4KB=2^12
        unsigned long long start_addr = LOW_MEM + i * current_index_page_size; // 根据页框块大小，空闲内存区开始地址来计算每个页框块的起始地址

        // 创建一个新的buddy结构体，并初始化它
        struct buddy *new_buddy = (struct buddy *)(start_addr);
        new_buddy->index = index;
        new_buddy->next = NULL; // 新buddy的next指针初始化为NULL

        // 如果当前索引的链表为空，直接将新buddy设置为链表的头
        if (FreePageBlock_in_List[index] == NULL) {
            FreePageBlock_in_List[index] = new_buddy;
        } else {
            // 如果链表不为空，找到链表的末尾并添加新buddy
            struct buddy *list_tail = FreePageBlock_in_List[index]; // 使用一个临时变量来保存链表的头
            while (list_tail->next != NULL) {
                list_tail = list_tail->next;
            }
            list_tail->next = new_buddy; // 将新buddy添加到链表的末尾
        }
    }
    return;
}

void buddy_system_init() {
    kputs("\n\n----------------------------------\n");
    kputs("buddy system MM start now\n");
    for (unsigned int i = 0; i < MAX_INDEX; ++i) 
    {
        FreePageBlock_in_List[i] = NULL;  ///初始化置空
    }

    list_init(10,22);

    FreePageBlock_in_List_Info();
    kputs("\ninit complete!\n");
    kputs("----------------------------------------------\n");
}


void free_pageBlock(void *addr, unsigned int index) {
    kputs("\n\n-------------------------------------------\n");
    kputs(" Buddy System free memory now！\n");
    struct buddy *occupied_start_addr = (struct buddy *)addr;  ///分裂之后被分配的内存块的起始地址
    occupied_start_addr->index = index;

    while (index < MAX_INDEX - 1) 
    {
        long long unsigned offset_addr = (((char *)occupied_start_addr - (char *)LOW_MEM) ^ (1 << (index + 12)));
        struct buddy *split_vacant_start_addr = (struct buddy *)((char *)LOW_MEM + offset_addr);  ///找到被分裂回收的伙伴内存块起始地址
        struct buddy **find_vacant_addr = &FreePageBlock_in_List [index]; 

        while (*find_vacant_addr && *find_vacant_addr != split_vacant_start_addr) 
        {
            find_vacant_addr = &(*find_vacant_addr)->next;
        }

        if (*find_vacant_addr != split_vacant_start_addr)
         {
            break;
        }  //没有break就是在块链表中找到了

        *find_vacant_addr = split_vacant_start_addr->next; 
        if (occupied_start_addr > split_vacant_start_addr) {
            occupied_start_addr = split_vacant_start_addr;
        }
        ++index;
    }

    occupied_start_addr->index = index;
    occupied_start_addr->next = FreePageBlock_in_List[index]; ///把原本分配出去的内存块的地址(已经合并过了)，加入原来的块链表中

    FreePageBlock_in_List [index] = occupied_start_addr;
    kprintf("the address of the freed page blocks : %p\n", addr);

    FreePageBlock_in_List_Info ();
}

void buddy_system_exe() {
    buddy_system_init();
    ///分配分裂
    kputs("-------------- Buddy is executing now! -------------\n\n");
    void *dist_example1 = memory_distribution(10); 
    kputs("---------------------------------------------------\n\n");

    ///分配分裂
    void *dist_example2 = memory_distribution(2); 
    kputs("----------------------------------------------------\n\n");

    // 释放合并
    free_pageBlock(dist_example1, 10); 
    kputs("--------------------------------------------------\n\n");
    
    ///分配分裂
    void *dist_example4 = memory_distribution(7);
    kputs("--------------------------------------------------------\n\n");

    // 释放合并
    free_pageBlock(dist_example4, 7);
    kputs("-----------------------------------------------------\n\n");

    // 释放合并
    free_pageBlock(dist_example2, 2); 
    kputs("-------------------------------------------------\n\n");


    kputs("end now !\n");
    kputs("you are the most excellent person in the world!\n");
    kputs("press 'ctrl + a' ,then press 'x',if you wanna exit !\n");
}