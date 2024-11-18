#ifndef BUDDY
#define BUDDY

#include <stddef.h>

#define MAX_INDEX 11   /// 1, 2, 4, ..., 1024 共11个块链表

struct buddy {
    unsigned int index;   /// 指数，2^index
    struct buddy *next;   /// 指向下一个空闲块
};

extern struct buddy *FreePageBlock_in_List[MAX_INDEX];  /// 指向每个块链表中第一个空闲页框块地址(指向头节点)

/**
 * @brief 初始化buddy内存系统。
 */
void buddy_system_init(void);

/**
 * @brief 从buddy中分配内存。
 *
 * @param index 要分配内存的索引。
 * @return 分配的内存块的指针，如果分配失败则返回NULL。
 */
void *memory_distribution(unsigned int index);

/**
 * @brief 将内存释放回buddy。
 *
 * @param addr 要释放的内存的地址。
 * @param index 正在释放的内存块的索引。
 */
void free_pageBlock(void *addr, unsigned int index);

/**
 * @brief 打印buddy空闲列表的当前状态。
 */
void FreePageBlock_in_List_Info(void);

/**
 * @brief 初始化空闲内存块链表。
 *
 * @param index 要初始化的块链表的索引。
 * @param loop 要初始化的块链表的数量。
 */
void list_init(unsigned int index, unsigned int loop);

/**
 * @brief 测试buddy系统。
 */
void buddy_system_exe(void);

#endif