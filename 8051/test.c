#include <8051.h>

// 假设晶振为 11.0592MHz，波特率 9600
void uart_init() {
    SCON = 0x50;  // 模式1, 8位数据, 可接收
    TMOD |= 0x20; // 定时器1，模式2 (8位自动重装)
    TH1 = 0xFD;   // 9600 波特率
    TL1 = 0xFD;
    TR1 = 1;      // 启动定时器1
    TI = 1;       // 设置发送中断标志以允许打印
}

void putchar(char c) {
    while (!TI);  // 等待上一个字节发送完成
    TI = 0;
    SBUF = c;
}

void print_str(char *s) {
    while (*s) {
        putchar(*s++);
    }
}

void delay(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void main() {
    uart_init();
    while (1) {
        print_str("Hello from Arch Linux!\r\n");
        delay(1000);
    }
}
