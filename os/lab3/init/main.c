#include <sbi.h>
#include <kdebug.h>
#include <mm.h>
#include <buddy.h>

int main()
{
    kputs("\nLZU OS STARTING....................");
    print_system_infomation();
    mem_init();
    mem_test();
    kputs("Hello LZU OS");

    buddy_system_exe();         // 测试 buddy 系统
    while (1)
        ; /* infinite loop */
    return 0;
}