#include <riscv.h>
#include <clock.h>
#include <sbi.h>
#include <kdebug.h>
#include <trap.h>
#include <plic.h>
#include <uart.h>
#include <rtc.h>
#include <dtb.h>


// void fault_fetch_exception()   ///引发 trap.c的 exception_handler函数中的case CAUSE_FAULT_FETCH
// {
//     volatile uint32_t* illegal_address = (uint32_t*)0x1; // 非法地址，通常是内核空间或未映射的内存
//     uint32_t value = *illegal_address; // 尝试从非法地址读取数据
//     kprintf("This line will not be executed due to fault fetch.\n"); // 这一行不会被执行，因为前面的指令会触发异常
//     return;
//     //在这个函数中，定义了一个指向非法地址 0x1 的指针
//     //（在大多数系统中，这个地址是不允许用户空间程序访问的）。
//     //然后尝试从这个地址读取数据，这将触发一个 FAULT_FETCH 异常。
// }

void fault_fetch_exception() {
    // 引发 CAUSE_FAULT_FETCH 异常
    uint32_t value;
    __asm__ __volatile__(
        "la a0, %c1\n"       // 加载标签的地址到寄存器a0中
        "lw a1, 0(a0)\n"      // 从a0指向的地址加载一个字到寄存器a1
        : "=r" (value)       // 输出操作数，将结果存储在value变量中
        : "i" (0x1)          // 输入操作数，将立即数0x1作为标签地址
        : "memory"           // 表示指令会改变内存状态
    );
    kprintf("This line will not be executed due to fault fetch.\n");
    return;
    //     //在这个函数中，定义了一个指向非法地址 0x1 的指针
    //     //（在大多数系统中，这个地址是不允许用户空间程序访问的）。
    //     //然后尝试从这个地址读取数据，这将触发一个 FAULT_FETCH 异常。
}


void ecall_exception()
{
    kprintf("This is  ecall 1 , print a number:\n");
    int num = 65; // 定义一个整数变量
  __asm__ __volatile__(
    "mv a0, %0\n" // 将整数变量的值移动到a0寄存器中
    "li a7, 1\n" // 设置系统调用编号为1，表示打印整数
    "ecall\n" // 执行ecall指令，请求服务
    : // 输出操作数，无
    : "r"(num) // 输入操作数，指定寄存器
    : "a0", "a7" // 破坏列表，指定使用的寄存器
  );
  
  kprintf("\necall end\n"); 
  return ;
}

int main(void *nothing, const void *dtb_start)
{
    kputs("\nLZU OS STARTING....................");
    print_system_infomation();
    kputs("Hello LZU OS");

    set_stvec();
    clock_init();
    kprintf("complete timer init\n");
    //dtb_start = (const void *)0x82200000;
    unflatten_device_tree(dtb_start);

    plic_init();
    kprintf("complete plic init\n");
    uart_init();
    kprintf("complete uart init\n");

    rtc_init();
    kprintf("timestamp now: %u\n", read_time());
    set_alarm(read_time() + 1000000000);
    kprintf("alarm time: %u\n", read_alarm());

    enable_interrupt(); // 启用 interrupt，sstatus的SSTATUS_SIE位置1

    __asm__ __volatile__("ebreak \n\t");
    ///ecall_exception(); 
    ///fault_fetch_exception();
    kputs("SYSTEM END");

    while (1)
        ; /* infinite loop */
    return 0;
}
